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

#include "Structure/SceneManager.hpp"
#include "System/LogError.hpp"

#include <string>

namespace ae {

	internal::SceneManagerType SceneManager = internal::CreateStructure<internal::SceneManagerType>();

	namespace internal {

		SceneManagerType::SceneManagerType() : m_next_scene(0, false), m_scene_popped(false, false), m_current_scene_id(1) {}

		void SceneManagerType::Terminate() {
			PopScene(true);
		}

		void SceneManagerType::NextScene(SceneId register_id, bool remove_active)
		{
			AE_ASSERT(register_id != 0, "Scene cannot be registered as 0");
			AE_ASSERT(IsSceneRegistered(register_id), "Scene '" << register_id << "' has not been registered yet")

			if (!IsSceneRegistered(register_id)) {
				LogError("[Aether] Scene " + std::to_string(register_id) + " does not exist in registry", true);
				return;
			}

			m_next_scene.first = register_id;
			m_next_scene.second = remove_active;
		}

		void SceneManagerType::UpdateActive() {

			if (m_scene_popped.first) {
				PopScene(m_scene_popped.second);
				m_scene_popped.first = false;
			}

			else if (m_next_scene.first != 0) {

				if (m_next_scene.second)
					PopScene(false);

				Scene* new_scene = m_scene_registry[m_next_scene.first-1]();
				m_scenes.push(new_scene);

				m_next_scene.first = 0;
			}
		}
		void SceneManagerType::PopScene(bool pop_all) {

			if (IsEmpty()) 
				return;

			delete m_scenes.top();
			m_scenes.pop();

			if (pop_all)
				while (!IsEmpty()) {
					delete m_scenes.top();
					m_scenes.pop();
				}
		}

		void SceneManagerType::FinishScene(bool finish_all) {
			m_scene_popped.first = true;
			m_scene_popped.second = finish_all;
		}

		bool SceneManagerType::IsSceneRegistered(SceneId register_id) const {
			return (register_id < m_scene_registry.size()+1);
		}

		bool SceneManagerType::IsEmpty() const {
			return m_scenes.empty();
		}

		Scene* SceneManagerType::GetActiveScene() const {
			if (IsEmpty()) return nullptr;
			return m_scenes.top();
		}
	}
}