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


#include "Structure/EntityComponentSystem/Entity.hpp"

static std::vector<ae::EntityGroup> RemoveDuplications(std::vector<ae::EntityGroup> vec) {
	for (auto it(vec.begin()); it != vec.end(); it++)
		for (auto it2(it + 1); it2 != vec.end();)
			if (*it == *it2) {
				it2 = vec.erase(it2);
			}
			else
				it2++;

	return std::move(vec);
}

namespace ae {

	void Entity::AddToGroup(EntityGroup group) {
		AE_ASSERT(!IsInGroup(group), "Entity already belongs to group '" << group << '\'');

		EntityManager.RegisterEntityToGroup(group, this);
		EntityManager.UpdateViews(this, m_groups, { group }, {});

		m_groups.push_back(group);
	}
	void Entity::RemoveFromGroup(EntityGroup group) {
		AE_ASSERT(group != AllEntitiesGroup, "Removing 'AllEntitiesGroup' (index 0) using Entity::RemoveFromGroup is forbidden");
		AE_ASSERT(IsInGroup(group), "Entity does not belong to group '" << group << '\'');

		EntityManager.UnregisterEntityFromGroup(group, this);
		EntityManager.UpdateViews(this, m_groups, {}, { group });

		m_groups.erase(std::find(m_groups.begin(), m_groups.end(), group));
	}

	void Entity::AddToGroups(const std::vector<EntityGroup>& groups) {

		std::vector<EntityGroup> new_groups = RemoveDuplications(groups);

		for (auto it(new_groups.begin()); it != new_groups.end();) {
			AE_ASSERT(*it != AllEntitiesGroup, "All entities belong to 'AllEntitiesGroup' (index 0), please do agree with it");

			if (std::find(m_groups.begin(), m_groups.end(), *it) != m_groups.end())
				it = new_groups.erase(it);
			else {
				EntityManager.RegisterEntityToGroup(*it, this);
				it++;
			}
		}
		EntityManager.UpdateViews(this, m_groups, new_groups, {});

		m_groups.insert(m_groups.end(), new_groups.begin(), new_groups.end());
	}
	void Entity::RemoveFromGroups(const std::vector<EntityGroup>& groups) {

		std::vector<EntityGroup> new_groups = RemoveDuplications(groups);

		for (auto it(new_groups.begin()); it != new_groups.end();) {
			AE_ASSERT(*it != AllEntitiesGroup, "All entities belong to 'AllEntitiesGroup' (index 0), please do agree with it");

			if (std::find(m_groups.begin(), m_groups.end(), *it) == m_groups.end())
				it = new_groups.erase(it);
			else {
				EntityManager.UnregisterEntityFromGroup(*it, this);
				it++;
			}
		}
		EntityManager.UpdateViews(this, m_groups, {}, new_groups);

		for (EntityGroup group : new_groups)
			m_groups.erase(std::find(m_groups.begin(), m_groups.end(), group));
	}

	bool Entity::IsInGroup(EntityGroup group) {
		AE_ASSERT(group != AllEntitiesGroup, "All entities belong to 'AllEntitiesGroup' (index 0), please do agree with it");
		return (std::find(m_groups.begin(), m_groups.end(), group) != m_groups.end());
	}
	bool Entity::IsInAllGroups(const std::vector<EntityGroup>& groups) {
		for (EntityGroup group : groups) {
			AE_ASSERT(group != AllEntitiesGroup, "All entities belong to 'AllEntitiesGroup' (index 0), please do agree with it");
			if (std::find(m_groups.begin(), m_groups.end(), group) == m_groups.end())
				return false;
		}

		return true;
	}
	bool Entity::IsInAnyGroup(const std::vector<EntityGroup>& groups) {
		for (EntityGroup group : groups) {
			AE_ASSERT(group != AllEntitiesGroup, "All entities belong to 'AllEntitiesGroup' (index 0), please do agree with it");
			if (std::find(m_groups.begin(), m_groups.end(), group) != m_groups.end())
				return true;
		}

		return false;
	}
}