////////////////////////////////////////////////////////////////////////////////////
// BSD 3-Clause License
// 
// Copyright (c) 2021, Wiktor Kasjaniuk
// All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
// 
// 1. Redistributions of source code must retain the above copyright notice, this
//    list of conditions and the following disclaimer.
// 
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
// 
// 3. Neither the name of the copyright holder nor the names of its
//    contributors may be used to endorse or promote products derived from
//    this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
// FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
// DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
// SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
// CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
// OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Entity Component System
///
/// General ideas:
/// 
///		EntityManager stores and manages all entiites and components.
///		Each entity can be assigned only one component of a type.
///		All components must publicly inherit from Component.
///		All components retrieve their entity pointers after the constructor runs, thus in order to use them during component initialization, override Initialize function
///		Each entity can belong to any of the created groups.
///		Each entity belong to AllEntitiesGroup (index 0), adding or removing them from it is not allowed.
///		Each entity that has been killed will be optainable through Viewing (Entity::IsAlive() can be used) until it is removed when Refresh function is called, which happens every application loop iteration, 
///		but can also be called any time. To disable this initialize the application with 'ecs_refresh_entities_each_tick' framework setting set to false.
/// 
/// Iterating through entities:
/// 
///		ViewComponent and ViewEntities are used to iterate through EntityManager's assets.
///		An empty storage is removed as soon as the last entity/component is removed from it.
///		Entities can be easily killed while viewing as they are removed every time Refresh function is called (defaultly every tick),
///		but removing them via RemoveAliveEntity can unleash undefined behaviour. This also applies to removing components while iterating through their type.
///		
///		ViewComponent
///			> iterates through components of a given type.
///		
///		ViewEntities 
///			> iterates through entities of a group, where group AllEntitiesGroup (index 0) stores all entities.
///			> iterates over advanced views (storages containing entities tagged by either (multiple groups and/or exclude groups) or (multiple components and/or exclude components)).
///		
///			If a tag does not exist EntityManager tries to create it's storage (advanced view) by iterating through all entities and checking if any belongs to it,
///			if no such entity was found, storage is not created. Thus viewing empty (same thing applies to sorting but NOT to clearing) advanced views can be resource-consuming, 
///			in order to control registering and unregistering advanced views manually initialize the application with 'ecs_manage_advanced_views_manually' framework setting set to true,
///			that also makes clearing EntityManager or it's storages not to remove advanced entity storages.
/// 
///		GetBeginEndIterators
///			Another way is to retrieve raw iterators, GetBeginEndIterators return pair of iterators to the vector of entities/components.
///			If such vector was not found, defaultly-initialized iterators are returned instead.
///			If an advanced view has not been registered yet, EntityManager will try to register it.
///			Although returned iterators are not const_iterators, it is not advised to erase any element
/// 
///	Clearing
///		Clear functions instantly erase all suitable entities. 
///		If clearing an advanced view that has not been registered yet,
///		EntityManager will iterate through all entities instead without registering it.
///
/// Sorting:
///		Sorts entities/components using std::sort function.
///		If sorting an advanced view that has not been registered yet,
///		EntityManager will try to register it.
/// 
/// Counting:
///		Returns the number of suitable entities.
///		If counting entities from an advanced view that has not been registered yet,
///		EntityManager will try to register it.
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../../Core/CreateStructure.hpp"
#include "../../Core/Preprocessor.hpp"

#include "Utility.hpp"
#include "ComponentPack.hpp"
#include "ViewTag.hpp"

#include <vector>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace ae {
	
	class Entity;
	class Component;

	namespace internal {

		class EntityManagerType {
			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<EntityManagerType>();

			friend class Entity;

		public:
			~EntityManagerType() { Clear(); }

			Entity& CreateEntity();
			EntityGroup CreateGroup();
			void RemoveAliveEntity(Entity& entity);
			void Refresh();

			template <typename ComponentType>
			void Clear();
			void Clear(EntityGroup group = AllEntitiesGroup);
			void Clear(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups = {});
			void Clear(const ComponentPack<>& components, const ComponentPack<>& exclude_components = {});

			template <typename ComponentType>
			void ViewComponents(const std::function<void(ComponentType&)>& operation);
			void ViewEntities(const std::function<void(Entity&)>& operation, EntityGroup group = AllEntitiesGroup);
			void ViewEntities(const std::function<void(Entity&)>& operation, const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups = {});
			void ViewEntities(const std::function<void(Entity&)>& operation, const ComponentPack<>& components, const ComponentPack<>& exclude_components = {});

			void RegisterAdvancedView(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups = {});
			void RegisterAdvancedView(const ComponentPack<>& components, const ComponentPack<>& exclude_components = {});

			void UnregisterAdvancedView(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups = {});
			void UnregisterAdvancedView(const ComponentPack<>& components, const ComponentPack<>& exclude_components = {});
			void UnregisterAllAdvancedViews();

			template <typename ComponentType>
			void SortView(const std::function<bool(ComponentType&, ComponentType&)>& compare);
			void SortView(const std::function<bool(Entity&, Entity&)>& compare, EntityGroup group = AllEntitiesGroup);
			void SortView(const std::function<bool(Entity&, Entity&)>& compare, const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups = {});
			void SortView(const std::function<bool(Entity&, Entity&)>& compare, const ComponentPack<>& components, const ComponentPack<>& exclude_components = {});
			
			template <typename ComponentType>
			std::pair<std::vector<Component*>::iterator, std::vector<Component*>::iterator> GetBeginEndIterators();
			std::pair<std::vector<Entity*>::iterator,    std::vector<Entity*>::iterator>    GetBeginEndIterators(EntityGroup group = AllEntitiesGroup);
			std::pair<std::vector<Entity*>::iterator,    std::vector<Entity*>::iterator>    GetBeginEndIterators(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups = {});
			std::pair<std::vector<Entity*>::iterator,    std::vector<Entity*>::iterator>    GetBeginEndIterators(const ComponentPack<>& components, const ComponentPack<>& exclude_components = {});

			template <typename ComponentType>
			size_t CountComponents() const;
			size_t CountComponents() const;
			size_t CountEntities(EntityGroup group = AllEntitiesGroup) const;
			size_t CountEntities(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups = {});
			size_t CountEntities(const ComponentPack<>& components, const ComponentPack<>& exclude_components = {});
			size_t CountAdvancedViews() const;

		private:
			std::unordered_map<ComponentTypeId, std::vector<Component*>> m_components;
			std::unordered_map<EntityGroup, std::vector<Entity*>> m_entities;
			std::unordered_map<GroupTag, std::vector<Entity*>, GroupTag::Hash> m_group_views;
			std::unordered_map<ComponentTag, std::vector<Entity*>, ComponentTag::Hash> m_component_views;

			EntityGroup m_next_group = AllEntitiesGroup;

			EntityManagerType() = default;
			EntityManagerType(const EntityManagerType&) = delete;
			EntityManagerType(EntityManagerType&&) = delete;

			void ClearAll();

			void PushComponent(Component* component);
			void EraseComponent(Component* component);

			void RegisterEntityToGroup(EntityGroup group, Entity* entity);
			void UnregisterEntityFromGroup(EntityGroup group, Entity* entity);
			
			// Advanced Views
			decltype(m_group_views)::iterator RegisterView(GroupTag&& tag);
			decltype(m_component_views)::iterator RegisterView(ComponentTag&& tag);

			decltype(m_component_views)::iterator FindOrRegisterView(ComponentTag&& tag);
			decltype(m_group_views)::iterator FindOrRegisterView(GroupTag&& tag);

			void UpdateViews(Entity* entity, const std::vector<EntityGroup>& old_groups, const std::vector<EntityGroup>& add_groups, const std::vector<EntityGroup>& remove_groups);
			void UpdateViews(Entity* entity, const std::vector<Component*>& old_components, Component* component, bool add_or_remove);

			void UpdateComponentViewsOnEntityRemoval(Entity* killed_entity);
			void UpdateComponentViewsOnEntityRemoval(Entity* killed_entity, const ComponentTag& ignore_tag);

			void UpdateGroupViewsOnEntityRemoval(Entity* killed_entity);
			void UpdateGroupViewsOnEntityRemoval(Entity* killed_entity, const GroupTag& ignore_tag);

			decltype(m_group_views)::iterator UnregisterFromView(Entity* entity, decltype(m_group_views)::iterator view);
			decltype(m_component_views)::iterator UnregisterFromView(Entity* entity, decltype(m_component_views)::iterator view);

			void UnregisterView(decltype(m_group_views)::iterator view);
			void UnregisterView(decltype(m_component_views)::iterator view);
		};

		// Template Definitions

		template <typename ComponentType>
		void EntityManagerType::Clear() {

			AE_ASSERT((std::is_base_of<Component, ComponentType>::value), "Could not view components, ComponentType must inherit from Component");

			// find
			auto location = m_components.find(typeid(ComponentType));
			if (location == m_components.end())
				return;

			// iterate through components
			for (auto comp_it(location->second.begin()); comp_it != location->second.end(); comp_it++) {
				Entity* entity = (*comp_it)->GetEntity();

				// remove from views
				UpdateGroupViewsOnEntityRemoval(entity);
				UpdateComponentViewsOnEntityRemoval(entity);

				// remove from groups
				UnregisterEntityFromGroup(AllEntitiesGroup, entity);

				for(EntityGroup e_group : entity->m_groups)
					UnregisterEntityFromGroup(e_group, entity);

				// delete components
				for (Component* e_comp : entity->m_components) {

					// ignore clear component type's component
					if (typeid(*e_comp) != typeid(**comp_it))
						EraseComponent(e_comp);
				}

				// clear heap space
				delete entity;
				delete* comp_it;
			}

			// clean up
			m_components.erase(location);
		}

		template <typename ComponentType>
		void EntityManagerType::ViewComponents(const std::function<void(ComponentType&)>& operation) {

			AE_ASSERT((std::is_base_of<Component, ComponentType>::value), "Could not view components, ComponentType must inherit from Component");

			auto location = m_components.find(typeid(ComponentType));

			// check if exists
			if (location == m_components.end())
				return;

			// iterate
			for (Component* comp : location->second)
				operation(*static_cast<ComponentType*>(comp));
		}

		template <typename ComponentType>
		void EntityManagerType::SortView(const std::function<bool(ComponentType&, ComponentType&)>& compare) {

			AE_ASSERT((std::is_base_of<Component, ComponentType>::value), "Could not sort components, ComponentType must inherit from Component");

			// find
			auto location = m_components.find(typeid(ComponentType));

			if (location == m_components.end())
				return;

			// sort
			std::sort(location->second.begin(), location->second.end(),
				[&compare](Component* comp1, Component* comp2) -> bool {
					return compare(*static_cast<ComponentType*>(comp1), *static_cast<ComponentType*>(comp2));
			});
		}

		template <typename ComponentType>
		std::pair< std::vector<Component*>::iterator, std::vector<Component*>::iterator> EntityManagerType::GetBeginEndIterators() {

			AE_ASSERT((std::is_base_of<Component, ComponentType>::value), "Could not retrieve iterators to components' vector, ComponentType must inherit from Component");

			// find
			auto location = m_components.find(typeid(ComponentType));

			if (location == m_components.end())
				return {};

			// return
			return { location->second.begin(), location->second.end() };
		}

		template <typename ComponentType>
		size_t EntityManagerType::CountComponents() const {
			AE_ASSERT((std::is_base_of<Component, ComponentType>::value), "Could not count components, ComponentType must inherit from Component");
			
			// find
			auto it = m_components.find(typeid(ComponentType));
			if (it == m_components.end())
				return 0;
		
			// return
			return it->second.size();
		}
	}

	extern internal::EntityManagerType EntityManager;
}