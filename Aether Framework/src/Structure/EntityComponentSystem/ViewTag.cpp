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

#include "Structure/EntityComponentSystem/ViewTag.hpp"

namespace ae {

	namespace internal {
		bool GroupTag::operator==(const GroupTag& other) const {
			return (include == other.include && exclude == other.exclude);
		}
		bool GroupTag::operator!=(const GroupTag& other) const {
			return !(include == other.include && exclude == other.exclude);
		}
		bool GroupTag::IsCompatible(const std::vector<EntityGroup>& groups) const {

			for (EntityGroup incl_group : include)
				if (std::find(groups.begin(), groups.end(), incl_group) == groups.end())
					return false;

			for (EntityGroup group : groups)
				if (std::find(exclude.begin(), exclude.end(), group) != exclude.end())
					return false;

			return true;
		}

		size_t GroupTag::Hash::operator()(const GroupTag& tag) const {
			size_t hash = 0;

			for (EntityGroup group : tag.include)
				hash ^= group + 0x9e3779b9 + (hash << 6) + (hash >> 2);

			for (EntityGroup group : tag.exclude)
				hash ^= group + 0x9e3779b9 + (hash << 7) + (hash >> 3);

			return hash;
		}

		bool ComponentTag::operator==(const ComponentTag& other) const {
			return (include == other.include && exclude == other.exclude);
		}
		bool ComponentTag::operator!=(const ComponentTag& other) const {
			return !(include == other.include && exclude == other.exclude);
		}
		
		bool ComponentTag::IsCompatible(const std::vector<Component*>& components) const {
			for (const ComponentTypeId& incl_comp : include)

				if (std::find_if(components.begin(), components.end(), 
					[&incl_comp](Component* comp) {
						return (ComponentTypeId(typeid(*comp)) == incl_comp);
					}) == components.end())

					return false;

			for (const Component* component : components)
				if (std::find(exclude.begin(), exclude.end(), typeid(*component)) != exclude.end())
					return false;

			return true;
		}

		size_t ComponentTag::Hash::operator()(const ComponentTag& tag) const {
			size_t hash = 0;

			for (const ComponentTypeId& component : tag.include)
				hash ^= component.hash_code() + 0x9e3779b9 + (hash << 6) + (hash >> 2);

			for (const ComponentTypeId& component : tag.exclude)
				hash ^= component.hash_code() + 0x9e3779b9 + (hash << 7) + (hash >> 3);

			return hash;
		}
		
	}
}