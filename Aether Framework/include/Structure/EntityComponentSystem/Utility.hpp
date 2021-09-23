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

#include <typeindex>

#define AE_ECS_ADVANCED_COMP_VIEW_DEBUG_LOGIC_TEST(include, exclude, invalid_size_message, share_element_message) \
 \
	AE_DEBUG_ONLY ( \
		AE_ASSERT(include.size() > 1 || exclude.size() > 0, invalid_size_message); \
 \
		for (auto& include_element : include) \
			if (std::find(exclude.begin(), exclude.end(), include_element) != exclude.end()) { \
				AE_ASSERT_FALSE(share_element_message); \
			} \
	);

#define AE_ECS_ADVANCED_GROUP_VIEW_DEBUG_LOGIC_TEST(include, exclude, invalid_size_message, share_element_message) \
	AE_DEBUG_ONLY( \
		AE_ASSERT( \
			(include.find(AllEntitiesGroup) == include.end() && exclude.find(AllEntitiesGroup) == exclude.end()), \
			"groups and exclude_groups cannot possess AllEntitiesGroup (index 0)" \
		); \
		 \
		AE_ECS_ADVANCED_COMP_VIEW_DEBUG_LOGIC_TEST(include, exclude, invalid_size_message, share_element_message) \
	);

namespace ae {
	namespace internal { 
		typedef std::type_index ComponentTypeId;
	} 

	typedef size_t EntityGroup;
	constexpr EntityGroup AllEntitiesGroup = 0;
}