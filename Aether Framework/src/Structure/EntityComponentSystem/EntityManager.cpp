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

#include "Core/Preprocessor.hpp"
#include "Structure/EntityComponentSystem/EntityManager.hpp"
#include "Structure/EntityComponentSystem/Entity.hpp"
#include "Structure/Application.hpp"

namespace ae {
	internal::EntityManagerType EntityManager = internal::CreateStructure<internal::EntityManagerType>();

	namespace internal {

//////// Registering ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void EntityManagerType::PushComponent(Component* component) {
			ComponentTypeId id = typeid(*component);

			// find vec
			auto it = m_components.find(id);

			// push to vec
			if (it == m_components.end())
				// create if needed
				m_components.insert({ id, {component} });
			else
				it->second.push_back(component);
		}
		void EntityManagerType::EraseComponent(Component* component) {

			// find
			auto it = m_components.find(typeid(*component));
			auto location = std::find(it->second.begin(), it->second.end(), component);

			// delete
			delete *location;
			it->second.erase(location);

			// clean up
			if (it->second.empty())
				m_components.erase(it);
		}

		void EntityManagerType::RegisterEntityToGroup(EntityGroup group, Entity* entity) {
			
			// find vec
			auto it = m_entities.find(group);

			// push to vec
			if (it == m_entities.end())
				// create if needed
				m_entities.insert({ group, {entity} });
			else
				it->second.push_back(entity);
		}

		void EntityManagerType::UnregisterView(decltype(m_group_views)::iterator view) {
			if (g_framework_settings.ecs_manage_advanced_views_manually)
				view->second.clear();
			else
				m_group_views.erase(view);
		}
		void EntityManagerType::UnregisterView(decltype(m_component_views)::iterator view) {
			if (g_framework_settings.ecs_manage_advanced_views_manually)
				view->second.clear();
			else
				m_component_views.erase(view);
		}

		void EntityManagerType::UnregisterEntityFromGroup(EntityGroup group, Entity* entity) {
			
			//find
			auto it = m_entities.find(group);

			// erase
			it->second.erase(std::find(it->second.begin(), it->second.end(), entity));

			// clean up
			if (it->second.empty())
				m_entities.erase(it);
		}

		decltype(EntityManagerType::m_group_views)::iterator EntityManagerType::UnregisterFromView(Entity* entity, decltype(m_group_views)::iterator view) {
			auto it = std::find(view->second.begin(), view->second.end(), entity);

			view->second.erase(it);

			if (g_framework_settings.ecs_manage_advanced_views_manually && view->second.empty())
				return m_group_views.erase(view);

			return ++view;
		}
		decltype(EntityManagerType::m_component_views)::iterator EntityManagerType::UnregisterFromView(Entity* entity, decltype(m_component_views)::iterator view) {
			auto it = std::find(view->second.begin(), view->second.end(), entity);

			view->second.erase(it);

			if (g_framework_settings.ecs_manage_advanced_views_manually && view->second.empty())
				return m_component_views.erase(view);

			return ++view;
		}
		decltype(EntityManagerType::m_group_views)::iterator EntityManagerType::RegisterView(GroupTag&& tag) {
			if (m_entities.empty())
				return m_group_views.end();

			std::vector<Entity*> view;

			// iterates through all entities to retrieve them in creation order
			for (Entity* entity : m_entities.at(AllEntitiesGroup))
				if (tag.IsCompatible(entity->m_groups))
					view.push_back(entity);

			if (view.empty())
				return m_group_views.end();

			return m_group_views.insert(
				std::make_pair<GroupTag, std::vector<Entity*>>(std::move(tag), std::move(view))
			).first;
		}
		decltype(EntityManagerType::m_component_views)::iterator EntityManagerType::RegisterView(ComponentTag&& tag) {
			if (m_entities.empty())
				return m_component_views.end();

			std::vector<Entity*> view;

			// iterates through all entities to retrieve them in creation order
			for (Entity* entity : m_entities.at(AllEntitiesGroup))
				if (tag.IsCompatible(entity->m_components))
					view.push_back(entity);

			if (view.empty())
				return m_component_views.end();

			return m_component_views.insert(
				std::make_pair<ComponentTag, std::vector<Entity*>>(std::move(tag), std::move(view))
			).first;
		}
		decltype(EntityManagerType::m_component_views)::iterator EntityManagerType::FindOrRegisterView(ComponentTag&& tag) {
			auto location = m_component_views.find(tag);

			if (location == m_component_views.end()) {

				AE_ASSERT(!g_framework_settings.ecs_manage_advanced_views_manually, "Could not find advanced view, it has not been registered yet")

				return RegisterView(std::move(tag));
			}
			return location;
		}
		decltype(EntityManagerType::m_group_views)::iterator EntityManagerType::FindOrRegisterView(GroupTag&& tag) {
			auto location = m_group_views.find(tag);

			if (location == m_group_views.end()) {

				AE_ASSERT(!g_framework_settings.ecs_manage_advanced_views_manually, "Could not find advanced view, it has not been registered yet")

				return RegisterView(std::move(tag));
			}
			return location;
		}

//////// UpdateViews ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void EntityManagerType::UpdateViews(Entity* entity, const std::vector<EntityGroup>& old_groups, const std::vector<EntityGroup>& add_groups, const std::vector<EntityGroup>& remove_groups) {
			
			// create new_groups for compatibility checks
			std::vector<EntityGroup> new_groups = old_groups;
			new_groups.insert(new_groups.end(), add_groups.begin(), add_groups.end());

			for (EntityGroup group : remove_groups) {
				auto it = std::find(new_groups.begin(), new_groups.end(), group);
				if (it != new_groups.end())
					new_groups.erase(it);
			}

			// update
			for (auto view(m_group_views.begin()); view != m_group_views.end();) {
				
				if (view->first.IsCompatible(old_groups)) {
						
					if (!view->first.IsCompatible(new_groups)) {
								
						view = UnregisterFromView(entity, view); \
						continue; 
					} 
				} 
				else if (view->first.IsCompatible(new_groups))
					view->second.push_back(entity); 
						
				view++; 
			}
			
		}

		void EntityManagerType::UpdateViews(Entity* entity, const std::vector<Component*>& old_components, Component* component, bool add_or_remove) {
			
			// create new_components for compatibility checks
			std::vector<Component*> new_components = old_components;

			if (add_or_remove)
				new_components.push_back(component);
			else
				new_components.erase(std::find(new_components.begin(), new_components.end(), component));

			// update
			for (auto view(m_component_views.begin()); view != m_component_views.end();) {

				if (view->first.IsCompatible(old_components)) {

					if (!view->first.IsCompatible(new_components)) {

						view = UnregisterFromView(entity, view); \
							continue;
					}
				}
				else if (view->first.IsCompatible(new_components))
					view->second.push_back(entity);

				view++;
			}
		}
		void EntityManagerType::UpdateGroupViewsOnEntityRemoval(Entity* killed_entity) {

			// component views
			for (auto view(m_group_views.begin()); view != m_group_views.end();) {

				if (view->first.IsCompatible(killed_entity->m_groups))
					view = UnregisterFromView(killed_entity, view);
				else
					view++;
			}
		}
		void EntityManagerType::UpdateComponentViewsOnEntityRemoval(Entity* killed_entity) {

			// component views
			for (auto view(m_component_views.begin()); view != m_component_views.end();) {

				if (view->first.IsCompatible(killed_entity->m_components))
					view = UnregisterFromView(killed_entity, view);
				else
					view++;
			}
		}
		void EntityManagerType::UpdateComponentViewsOnEntityRemoval(Entity* killed_entity, const ComponentTag& ignore_tag) {

			// component views
			for (auto view(m_component_views.begin()); view != m_component_views.end();) {

				if (view->first.IsCompatible(killed_entity->m_components) && view->first != ignore_tag)
					view = UnregisterFromView(killed_entity, view);
				else
					view++;
			}
		}
		void EntityManagerType::UpdateGroupViewsOnEntityRemoval(Entity* killed_entity, const GroupTag& ignore_tag) {

			// group views
			for (auto view(m_group_views.begin()); view != m_group_views.end();) {

				if (view->first.IsCompatible(killed_entity->m_groups) && view->first != ignore_tag)
					view = UnregisterFromView(killed_entity, view);
				else
					view++;
			}
		}
		
//////// Advanced View Manual Registering ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void EntityManagerType::RegisterAdvancedView(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups) {

			AE_ASSERT(g_framework_settings.ecs_manage_advanced_views_manually, "Could not register advanced view, 'ecs_manage_advanced_views_manually' was not set to true when initializing the application");

			GroupTag tag = { groups, exclude_groups };

			if (m_group_views.find(tag) != m_group_views.end())
				return;

			std::vector<Entity*> view;

			// iterates through all entities to retrieve them in creation order
			if (!m_entities.empty()) {
				for (Entity* entity : m_entities.at(AllEntitiesGroup))
					if (tag.IsCompatible(entity->m_groups))
						view.push_back(entity);
			}

			m_group_views.insert(std::make_pair<GroupTag, std::vector<Entity*>>(std::move(tag), std::move(view)));
		}
		void EntityManagerType::RegisterAdvancedView(const ComponentPack<>& components, const ComponentPack<>& exclude_components) {

			AE_ASSERT(g_framework_settings.ecs_manage_advanced_views_manually, "Could not register advanced view, 'ecs_manage_advanced_views_manually' was not set to true when initializing the application");

			ComponentTag tag = { components.m_components, exclude_components.m_components };

			if (m_component_views.find(tag) != m_component_views.end())
				return;

			std::vector<Entity*> view;

			// iterates through all entities to retrieve them in creation order
			if (!m_entities.empty()) {
				for (Entity* entity : m_entities.at(AllEntitiesGroup))
					if (tag.IsCompatible(entity->m_components))
						view.push_back(entity);
			}

			m_component_views.insert(std::make_pair<ComponentTag, std::vector<Entity*>>(std::move(tag), std::move(view)));
		}


		void EntityManagerType::UnregisterAdvancedView(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups) {

			AE_ASSERT(g_framework_settings.ecs_manage_advanced_views_manually, "Could not unregister advanced view, 'ecs_manage_advanced_views_manually' was not set to true when initializing the application");

			GroupTag tag = { groups, exclude_groups };

			if (m_group_views.find(tag) != m_group_views.end())
				m_group_views.erase(tag);
		}
		void EntityManagerType::UnregisterAdvancedView(const ComponentPack<>& components, const ComponentPack<>& exclude_components) {

			AE_ASSERT(g_framework_settings.ecs_manage_advanced_views_manually, "Could not unregister advanced view, 'ecs_manage_advanced_views_manually' was not set to true when initializing the application");

			ComponentTag tag = { components.m_components, exclude_components.m_components };

			if (m_component_views.find(tag) != m_component_views.end())
				m_component_views.erase(tag);
		}
		void EntityManagerType::UnregisterAllAdvancedViews() {

			AE_ASSERT(g_framework_settings.ecs_manage_advanced_views_manually, "Could not unregister advanced views, 'ecs_manage_advanced_views_manually' was not set to true when initializing the application");

			m_group_views.clear();
			m_component_views.clear();
		}

//////// Entity & Group Creation ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		Entity& EntityManagerType::CreateEntity() {
			Entity* entity = new Entity();
			RegisterEntityToGroup(AllEntitiesGroup, entity);
			return *entity;
		}

		EntityGroup EntityManagerType::CreateGroup() {
			return ++m_next_group;
		}

//////// Clear & Remove ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void EntityManagerType::RemoveAliveEntity(Entity& entity) {

			UpdateGroupViewsOnEntityRemoval(&entity);
			UpdateComponentViewsOnEntityRemoval(&entity);

			auto& entities = m_entities.at(AllEntitiesGroup);

			for (auto it(entities.begin()); it != entities.end(); it++)
				if (*it == &entity) {

					// delete components
					for (Component* comp : entity.m_components)
						EraseComponent(comp);

					// remove from groups
					UnregisterEntityFromGroup(AllEntitiesGroup, &entity);

					for (EntityGroup group : entity.m_groups)
						UnregisterEntityFromGroup(group, &entity);

					// delete entity
					delete& entity;
					break;
				}
		}

		void EntityManagerType::ClearAll() {
			
			if (g_framework_settings.ecs_manage_advanced_views_manually) {
				for (auto& view : m_group_views)
					view.second.clear();

				for (auto& view : m_component_views)
					view.second.clear();
			}
			else {
				m_group_views.clear();
				m_component_views.clear();
			}

			if (!m_entities.empty())
				for (auto& e : m_entities.at(AllEntitiesGroup))
					delete e;

			m_entities.clear();

			for (auto& p : m_components)
				for (auto& c : p.second)
					delete c;

			m_components.clear();
		}

		void EntityManagerType::Clear(EntityGroup group) {

			// find
			auto location = m_entities.find(group);

			if (location == m_entities.end())
				return;

			// AllEntitiesGroup - special case
			if (location->first == AllEntitiesGroup) {
				ClearAll();
				return;
			}

			// iterate through entities
			for (auto ent_it(location->second.begin()); ent_it != location->second.end(); ent_it++) {

				Entity* entity = *ent_it;

				// remove from views
				UpdateGroupViewsOnEntityRemoval(entity);
				UpdateComponentViewsOnEntityRemoval(entity);

				// remove from groups
				UnregisterEntityFromGroup(AllEntitiesGroup, entity);

				for (EntityGroup e_group : entity->m_groups){
					if (group != e_group)
						UnregisterEntityFromGroup(e_group, entity);
				}

				// delete components
				for (Component* e_comp : entity->m_components) {
					EraseComponent(e_comp);
				}

				// clean the heap space
				delete entity;
			}

			// clean up
			m_entities.erase(location);
		}
		void EntityManagerType::Clear(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups) {
			AE_ECS_ADVANCED_GROUP_VIEW_DEBUG_LOGIC_TEST(
				groups, exclude_groups,
				"Could not clear entities, invalid groups' sizes",
				"Could not clear entities, groups and exclude_groups shared at least one group"
			);

			GroupTag tag(groups, exclude_groups);

			// find
			auto location = m_group_views.find(tag);

			// view not registered - iterate through AllEntitiesGroup
			if (location == m_group_views.end()) {
				
				// find
				auto all_entities = m_entities.find(AllEntitiesGroup);

				if (all_entities == m_entities.end())
					return;

				// iterate through entities
				for (auto ent_it(all_entities->second.begin()); ent_it != all_entities->second.end();) {

					Entity* entity = *ent_it;
					
					// check if entity belongs to a view
					if (tag.IsCompatible(entity->m_groups)) {

						// remove from views
						UpdateGroupViewsOnEntityRemoval(entity);
						UpdateComponentViewsOnEntityRemoval(entity);

						// remove from groups
						for (EntityGroup e_group : entity->m_groups)
							UnregisterEntityFromGroup(e_group, entity);

						// delete components
						for (Component* e_comp : entity->m_components)
							EraseComponent(e_comp);

						// clean heap space
						delete entity;

						// next entity
						ent_it = all_entities->second.erase(ent_it);
					}
					else
						ent_it++;
				}
				return;
			}
			// view registered
			for (auto ent_it(location->second.begin()); ent_it != location->second.end(); ent_it++) {

				Entity* entity = *ent_it;

				// remove from views
				UpdateGroupViewsOnEntityRemoval(entity, tag);
				UpdateComponentViewsOnEntityRemoval(entity);

				// remove from groups
				UnregisterEntityFromGroup(AllEntitiesGroup, entity);

				for (EntityGroup e_group : entity->m_groups)
					UnregisterEntityFromGroup(e_group, entity);

				// delete components
				for (Component* e_comp : entity->m_components)
					EraseComponent(e_comp);

				// clean heap space
				delete entity;

			}
			
			// clean up
			UnregisterView(location);

		}
		void EntityManagerType::Clear(const ComponentPack<>& components, const ComponentPack<>& exclude_components) {
			AE_ECS_ADVANCED_COMP_VIEW_DEBUG_LOGIC_TEST(
				components.m_components, exclude_components.m_components,
				"Could not clear entities, invalid ComponentPacks' sizes",
				"Could not clear entities, components and exclude_components shared at least one group"
			);

			ComponentTag tag(components.m_components, exclude_components.m_components);

			// find
			auto location = m_component_views.find(tag);

			// view not registered - iterate through AllEntitiesGroup
			if (location == m_component_views.end()) {

				// find
				auto all_entities = m_entities.find(AllEntitiesGroup);

				if (all_entities == m_entities.end())
					return;

				// iterate through entities
				for (auto ent_it(all_entities->second.begin()); ent_it != all_entities->second.end();) {

					Entity* entity = *ent_it;

					// check if entity belongs to a view
					if (tag.IsCompatible(entity->m_components)) {

						// remove from views
						UpdateGroupViewsOnEntityRemoval(entity);
						UpdateComponentViewsOnEntityRemoval(entity);

						// remove from groups
						for (EntityGroup e_group : entity->m_groups)
							UnregisterEntityFromGroup(e_group, entity);

						// delete components
						for (Component* e_comp : entity->m_components)
							EraseComponent(e_comp);

						// clean heap space
						delete entity;

						// next entity
						ent_it = all_entities->second.erase(ent_it);
					}
					else
						ent_it++;
				}
				return;
			}
			// view registered
			for (auto ent_it(location->second.begin()); ent_it != location->second.end(); ent_it++) {

				Entity* entity = *ent_it;

				// remove from views
				UpdateGroupViewsOnEntityRemoval(entity);
				UpdateComponentViewsOnEntityRemoval(entity, tag);

				// remove from groups
				UnregisterEntityFromGroup(AllEntitiesGroup, entity);

				for (EntityGroup e_group : entity->m_groups)
					UnregisterEntityFromGroup(e_group, entity);

				// delete components
				for (Component* e_comp : entity->m_components)
					EraseComponent(e_comp);

				// clean heap space
				delete entity;

			}

			// clean up
			UnregisterView(location);
		}

		void EntityManagerType::Refresh() {
			if (m_entities.empty())
				return;

			auto& entities = m_entities.at(AllEntitiesGroup);

			for (auto it(entities.begin()); it != entities.end();) {
				if (!(*it)->IsAlive()) {
					Entity* entity = *it;

					// remove from views
					UpdateGroupViewsOnEntityRemoval(entity);
					UpdateComponentViewsOnEntityRemoval(entity);

					// delete components
					for (Component* comp : entity->m_components)
						EraseComponent(comp);

					// remove from groups
					for (EntityGroup group : entity->m_groups)
						UnregisterEntityFromGroup(group, entity);

					it = entities.erase(it);
					delete entity;
				}
				else
					it++;
			}
		}


//////// Views ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void EntityManagerType::ViewEntities(const std::function<void(Entity&)>& operation, EntityGroup group) {

			auto location = m_entities.find(group);

			// check if exists
			if (location == m_entities.end())
				return;

			// iterate
			for (Entity* entity : location->second)
				operation(*entity);
		}
		void EntityManagerType::ViewEntities(const std::function<void(Entity&)>& operation, const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups) {

			// check for logic
			AE_ECS_ADVANCED_GROUP_VIEW_DEBUG_LOGIC_TEST(
				groups, exclude_groups, 
				"Could not view entities, invalid groups' sizes", 
				"Could not view entities, groups and exclude_groups shared at least one group"
			);
			
			// find or register
			auto location = FindOrRegisterView(GroupTag(groups, exclude_groups));
			if (location == m_group_views.end())
				return;

			// iterate
			for (Entity* entity : location->second)
				operation(*entity);
		}
		void EntityManagerType::ViewEntities(const std::function<void(Entity&)>& operation, const ComponentPack<>& components, const ComponentPack<>& exclude_components) {
			
			// check for logic
			AE_ECS_ADVANCED_COMP_VIEW_DEBUG_LOGIC_TEST(
				components.m_components, exclude_components.m_components,
				"Could not create AdvancedEntityView, invalid ComponentPacks' sizes",
				"Could not create AdvancedEntityView, components and exclude_components share at least one component type"
			);

			// find or register
			auto location = FindOrRegisterView(ComponentTag(components.m_components, exclude_components.m_components));
			if (location == m_component_views.end())
				return;

			// iterate
			for (Entity* entity : location->second)
				operation(*entity);
		}

//////// Sorting ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		void EntityManagerType::SortView(const std::function<bool(Entity&, Entity&)>& compare, EntityGroup group) {
			
			// find
			auto it = m_entities.find(group);

			if (it == m_entities.end())
				return;

			// sort
			std::sort(it->second.begin(), it->second.end(),
				[&compare](Entity* entity1, Entity* entity2) -> bool {
					return compare(*entity1, *entity2);
			});

		}
		void EntityManagerType::SortView(const std::function<bool(Entity&, Entity&)>& compare, const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups) {
			
			// check for logic
			AE_ECS_ADVANCED_GROUP_VIEW_DEBUG_LOGIC_TEST(
				groups, exclude_groups,
				"Could not view entities, invalid groups' sizes",
				"Could not view entities, groups and exclude_groups shared at least one group"
			);

			// find or register
			auto location = FindOrRegisterView(GroupTag(groups, exclude_groups));
			if (location == m_group_views.end())
				return;

			// sort
			std::sort(location->second.begin(), location->second.end(),
				[&compare](Entity* entity1, Entity* entity2) -> bool {
					return compare(*entity1, *entity2);
			});

		}
		void EntityManagerType::SortView(const std::function<bool(Entity&, Entity&)>& compare, const ComponentPack<>& components, const ComponentPack<>& exclude_components) {
			// check for logic
			AE_ECS_ADVANCED_COMP_VIEW_DEBUG_LOGIC_TEST(
				components.m_components, exclude_components.m_components,
				"Could not create AdvancedEntityView, invalid ComponentPacks' sizes",
				"Could not create AdvancedEntityView, components and exclude_components share at least one component type"
			);

			// find or register
			auto location = FindOrRegisterView(ComponentTag(components.m_components, exclude_components.m_components));
			if (location == m_component_views.end())
				return;

			// sort
			std::sort(location->second.begin(), location->second.end(),
				[&compare](Entity* entity1, Entity* entity2) -> bool {
					return compare(*entity1, *entity2);
			});
		}

//////// Retrieve Iterators ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		std::pair<std::vector<Entity*>::iterator, std::vector<Entity*>::iterator> EntityManagerType::GetBeginEndIterators(EntityGroup group) {

			// find
			auto it = m_entities.find(group);

			if (it == m_entities.end())
				return {};

			// return
			return { it->second.begin(), it->second.end() };
		}
		std::pair< std::vector<Entity*>::iterator, std::vector<Entity*>::iterator> EntityManagerType::GetBeginEndIterators(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups) {

			// check for logic
			AE_ECS_ADVANCED_GROUP_VIEW_DEBUG_LOGIC_TEST(
				groups, exclude_groups,
				"Could not retrieve entity iterators, invalid groups' sizes",
				"Could not retrieve entity iterators, groups and exclude_groups shared at least one group"
			);

			// find or register
			auto location = FindOrRegisterView(GroupTag(groups, exclude_groups));
			if (location == m_group_views.end())
				return {};

			// return
			return { location->second.begin(), location->second.end() };
		}
		std::pair< std::vector<Entity*>::iterator, std::vector<Entity*>::iterator> EntityManagerType::GetBeginEndIterators(const ComponentPack<>& components, const ComponentPack<>& exclude_components) {
			// check for logic
			AE_ECS_ADVANCED_COMP_VIEW_DEBUG_LOGIC_TEST(
				components.m_components, exclude_components.m_components,
				"Could not retrieve entity iterators, invalid ComponentPacks' sizes",
				"Could not retrieve entity iterators, components and exclude_components share at least one component type"
			);

			// find or register
			auto location = FindOrRegisterView(ComponentTag(components.m_components, exclude_components.m_components));
			if (location == m_component_views.end())
				return {};

			// return
			return { location->second.begin(), location->second.end() };
		}

		size_t EntityManagerType::CountComponents() const {
			unsigned int count = 0;

			for (auto& p : m_components)
				count += p.second.size();

			return count;
		}

//////// Counting ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		size_t EntityManagerType::CountEntities(EntityGroup group) const {

			// find
			auto it = m_entities.find(group);
			if (it == m_entities.end())
				return 0;

			// return
			return it->second.size();
		}

		size_t EntityManagerType::CountEntities(const std::set<EntityGroup>& groups, const std::set<EntityGroup>& exclude_groups) {
			
			// find
			auto location = FindOrRegisterView(GroupTag(groups, exclude_groups));
			if (location == m_group_views.end())
				return 0;

			// return
			return location->second.size();

		}
		size_t EntityManagerType::CountEntities(const ComponentPack<>& components, const ComponentPack<>& exclude_components) {

			// find
			auto location = FindOrRegisterView(ComponentTag(components.m_components, exclude_components.m_components));
			if (location == m_component_views.end())
				return 0;

			// return
			return location->second.size();
		}
		size_t EntityManagerType::CountAdvancedViews() const {
			return m_group_views.size() + m_component_views.size();
		}
	}
}
