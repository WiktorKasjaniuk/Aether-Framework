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

#pragma once

#include "../../Core/Preprocessor.hpp"

#include "Utility.hpp"
#include "EntityManager.hpp"

#include <vector>

namespace ae {

	namespace internal {
		class EntityManagerType;
	}
	class Component;


	class Entity {
		friend class internal::EntityManagerType;

	public:
		~Entity() = default;

		template <typename ComponentType, typename ...ArgsType>
		ComponentType& AddComponent(ArgsType&&... args);

		template <typename ComponentType>
		void RemoveComponent();

		template <typename ComponentType>
		bool HasComponent() const;

		template <typename ComponentType>
		ComponentType& GetComponent() const;

		void AddToGroup(EntityGroup group);
		void RemoveFromGroup(EntityGroup group);

		void AddToGroups(const std::vector<EntityGroup>& groups);
		void RemoveFromGroups(const std::vector<EntityGroup>& groups);

		bool IsInGroup(EntityGroup group);
		bool IsInAllGroups(const std::vector<EntityGroup>& groups);
		bool IsInAnyGroup(const std::vector<EntityGroup>& groups);

		void Kill() { m_alive = false; }
		bool IsAlive() { return m_alive; }

	private:
		std::vector<Component*> m_components;
		std::vector<EntityGroup> m_groups; // does not store AllEntitiesGroup

		bool m_alive = true;

		Entity() = default;
		Entity(const Entity&) = delete;
		Entity(Entity&&) = delete;
	};

	// Template Definitions

	template <typename ComponentType, typename ...ArgsType>
	ComponentType& Entity::AddComponent(ArgsType&&... args) {

		AE_ASSERT(!HasComponent<ComponentType>(), "Entity already has '" << typeid(ComponentType).name() << "' component");

		ComponentType* comp = new ComponentType(std::forward<ArgsType>(args)...);
		comp->m_entity = this;
		comp->Initialize();

		EntityManager.PushComponent(comp);
		EntityManager.UpdateViews(this, m_components, comp, true);

		m_components.push_back(comp);

		return *comp;
	}

	template <typename ComponentType>
	void Entity::RemoveComponent() {

		AE_ASSERT(HasComponent<ComponentType>(), "Entity does not have '" << typeid(ComponentType).name() << "' component");

		// find & remove component from entity
		Component* comp = nullptr;

		// find 
		auto comp_it = std::find_if(m_components.begin(), m_components.end(),
			[](Component* comp) {
				return (typeid(*comp) == typeid(ComponentType));
		});

		// remove from manager
		EntityManager.UpdateViews(this, m_components, *comp_it, false);
		EntityManager.EraseComponent(*comp_it);

		// erase from entity
		m_components.erase(comp_it);
	}

	template <typename ComponentType>
	bool Entity::HasComponent() const {

		AE_ASSERT((std::is_base_of<Component, ComponentType>::value), "ComponentType must inherit from ae::Component, '" << typeid(ComponentType).name() <<"' does not");

		for (auto it(m_components.begin()); it != m_components.end(); it++)
			if (typeid(**it) == typeid(ComponentType))
				return true;

		return false;
	}

	template <typename ComponentType>
	ComponentType& Entity::GetComponent() const {

		AE_ASSERT(HasComponent<ComponentType>(), "Entity does not have '" << typeid(ComponentType).name() << "' component");

		for (auto it(m_components.begin()); it != m_components.end(); it++)
			if (typeid(**it) == typeid(ComponentType))
				return *static_cast<ComponentType*>(*it);

		// crash
		LogError("[Aether] Error while trying to return component '" + std::string(typeid(ComponentType).name()) + "', it does not belong to entity", true);
	}
}