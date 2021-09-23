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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Application
///	
/// This is the singleton managing main loop and initialization / termination of third party libraries.
/// 
/// Starting:
///		1. Initialize Application by passing first window's parameters to Initialize() function (ContextSettings and FrameworkSettings can be ignored),
///		2. Register first scene using SceneManager as Application cannot start without it,
///		3. Run() the Application.
/// 
/// Main Loop & Running:
///		1. Poll events,
///		2. Update the scene currently on top,
///		3. Draw layers if window is visible,
///		4. Refresh scenes (possibly jump to next)
///		5. Refresh entities (this can be turned off using a framework setting)
///		6. Update tick time
/// 
/// Closing:
///		The application will automatically close if SceneManager is empty or when Close() is called
/// 
/// Time:
///		GetTickTime() returns the time of the last iteration of the main loop,
///		GetRunTime() return time elapsed since begining of main loop.
/// 
/// FrameworkSettings:
///		Enable or disable different features.
/// 
///		ecs_refresh_entities_each_tick: (default value: true)
///			if set to true, EntityManager will stop being Refreshed every tick and function EntityManager.Refresh will have to be called,
/// 
///		ecs_manage_advanced_views_manually: (default value: false)
///			if set to true, makes the user responsible for managing advanced entity/entity group views, possible performance improvement,
/// 
///		log_errors: (default value: true)
///			if set to true, stops Aether Framework from outputting crashes if set to false,
/// 
///		log_errors_file: (default value: "error_log.txt")
///			defines the filename of error log file.
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Core/CreateStructure.hpp"

#include "../System/Vector2.hpp"
#include "../System/Time.hpp"
#include "../System/Events/Event.hpp"

#include "Window.hpp"

#include <string>

namespace ae {

	struct FrameworkSettings {
		bool ecs_refresh_entities_each_tick = true;
		bool ecs_manage_advanced_views_manually = false;
		bool log_errors = true;
		std::string log_errors_file = "error_log.txt";
	};

	namespace internal {

		class ApplicationType {
			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<ApplicationType>();
			
		public:
			~ApplicationType() = default;

			void Initialize(const Vector2i& window_size, const std::string& window_title, const ContextSettings& context_settings = ContextSettings(), const FrameworkSettings& framework_settings = FrameworkSettings());

			void Run();
			void Close();

			const Time& GetTickTime() const;
			Time GetRunTime() const;

			void SetCloseCallback(const std::function<void()>& callback);

		private:
			bool m_should_close;
			Time m_ticktime, m_start_time;

			std::function<void()> m_close_callback;

			ApplicationType();
			ApplicationType(const ApplicationType&) = delete;
			ApplicationType(ApplicationType&&) = delete;

			void Terminate();
		};

		extern FrameworkSettings g_framework_settings;
	}

	extern ae::internal::ApplicationType Application;
}