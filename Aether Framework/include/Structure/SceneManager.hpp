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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Scenes
///
/// General Idea:
///		Scene is an abstract class representing application's operations / updating purposes, in other words it is just a level.
///		Multiple scenes can be stored in memory, but ony one can be active at a time.
///		At least one scene has to exist for Application to be running.
///		
///	IsEmpty() returns true if there are no active scenes,
/// GetActiveScene() returns currently active scene, nullptr otherwise.
/// 
/// First Scene:
///		1. Application.Initialize(params);
///		2. SceneId scene = SceneManager.RegisterScene<MyScene>();
///		3. SceneManager.NextScene(scene);
///		4. Applcation.Run();
/// 
/// Registering:
///		Before using a scene, it's type has to be registered.
///		Only one scene type can be registered at a time, and RegisterScene() returns it's unique id.
///		You can check if a scene type has been registered using IsSceneRegistered(scene_id).
/// 
/// Changing Scenes:
///		NextScene(scene_id, remove_current = true) 
///			will change to a new scene as soon as the main loop ends,
///			and also remove currently active scene.
///		
///		FinishScene(bool finish_all = false)
///			will remove current scene as soon as the main loop ends.
///			If there is a 'frozen' / not removed scene below it, it will become the current scene,
///			if not, the application will end.
/// 
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Core/Preprocessor.hpp"
#include "../Core/CreateStructure.hpp"
#include "../System/Events/Event.hpp"

#include <functional>
#include <vector>
#include <stack>

namespace ae {

	class Scene {
	public:
		Scene() {};
		virtual ~Scene() {};

		virtual void Update() = 0;
	};

	typedef size_t SceneId;

	namespace internal {

		class ApplicationType;

		class SceneManagerType {
			friend class ae::internal::ApplicationType;

			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<SceneManagerType>();

		public:
			~SceneManagerType() = default;

			template<typename T>
			SceneId RegisterScene()
			{
				AE_DEBUG_ONLY(
					static bool type_registered = false;
					AE_ASSERT(!type_registered, "Scene of the same type as scene '" << m_current_scene_id << "' has already been registered");
					type_registered = true;
				);

				m_scene_registry.push_back([] { return new T(); });

				return ++m_current_scene_id-1;
			}

			void NextScene(SceneId register_id, bool remove_active = true);
			void FinishScene(bool finish_all = false);

			bool IsSceneRegistered(SceneId register_id) const;
			bool IsEmpty() const;

			Scene* GetActiveScene() const;

		private:
			std::stack<Scene*> m_scenes;

			std::vector<std::function<Scene*()>> m_scene_registry;
			SceneId m_current_scene_id;

			SceneManagerType();
			SceneManagerType(const SceneManagerType&) = delete;
			SceneManagerType(SceneManagerType&&) = delete;
			
			void Terminate(); 

			std::pair<SceneId, bool> m_next_scene;
			std::pair<bool, bool> m_scene_popped;

			void UpdateActive();
			void PopScene(bool pop_all);
		};
	}

	extern ae::internal::SceneManagerType SceneManager;
	
}


