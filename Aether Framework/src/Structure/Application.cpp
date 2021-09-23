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

#include <glad/glad.h>
#include <glad/glad.c>
#include <glfw/glfw3.h>

#include "Core/Preprocessor.hpp"
#include "Core/OpenGLCalls.hpp"

#include "System/Events/EventCallbacks.hpp"
#include "System/Clock.hpp"
#include "System/LogError.hpp"

#include "Structure/Application.hpp"
#include "Structure/SceneManager.hpp"
#include "Structure/AssetManager.hpp"
#include "Structure/Camera.hpp"
#include "Structure/LayerManager.hpp"
#include "Structure/Cursor.hpp"
#include "Structure/EntityComponentSystem/EntityManager.hpp"

#include "Graphics/Font.hpp"
#include "Audio/AudioDevice.hpp"

#include <iostream>
#include <stdlib.h>

namespace ae {

    internal::ApplicationType Application = internal::CreateStructure<internal::ApplicationType>();

    namespace internal {

        FrameworkSettings g_framework_settings;

        ApplicationType::ApplicationType()
            : m_should_close(true)
        {}

        const Time& ApplicationType::GetTickTime() const {
            return m_ticktime;
        }
        Time ApplicationType::GetRunTime() const {
            return Time(glfwGetTime()) - m_start_time;
        }

        void ApplicationType::Initialize(const Vector2i& window_size, const std::string& window_title, const ContextSettings& context_settings, const FrameworkSettings& framework_settings) {

            g_framework_settings = framework_settings;
            
            // initialize GLFW
            if (!glfwInit()) {
                AE_ASSERT_FALSE("Could not initialize GLFW, possible platform error");
                LogError("[Aether] Could not initialize GLFW, possible platform error");
                std::exit(EXIT_FAILURE);
            }

            AE_DEVELOP_ONLY(
                glfwSetErrorCallback([](int error, const char* description) {
                    std::cout << "[GLFW Error " << error << "]\n" << description << '\n';
                });
            );

            // create window
            Window.Initialize(window_size, window_title, context_settings);
            internal::SetEventCallbacks(Window.m_native_window);

            // camera
            Camera.UpdateCameraSize(Window.GetContextSize());
            Camera.SetPosition(Vector2f(Window.GetContextSize()) / 2.f);
            
            // initialize GLAD
            if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
                AE_ASSERT_FALSE("Could not initialize GLAD");
                LogError("[Aether] Could not initialize GLAD");
                std::exit(EXIT_FAILURE);
            }

            AE_GL_LOG(glEnable(GL_MULTISAMPLE));
            AE_GL_LOG(glEnable(GL_BLEND));
            AE_GL_LOG(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

            // Initialize Freetype
            bool ft_loaded = internal::InitializeFontLibrary();
            AE_ASSERT(ft_loaded, "Could not initialize FreeType")
            if (!ft_loaded) {
                LogError("[Aether] Could not initialize FreeType");
                std::exit(EXIT_FAILURE);
            }

            // set window parameters
            Window.SetClearColor(Color::Cavern);
            Window.SetVSync(false);

            // initialize audio
            AudioDevice::Initialize();

            // asset manager
            AssetManager.Initialize();

            // cursor
            Cursor.Initialize();

            // window initialized
            m_should_close = false;
        }

        void ApplicationType::Terminate() {
            SceneManager.Terminate();
            AssetManager.Terminate();
            internal::TerminateFontLibrary();
            Cursor.Terminate();
            Window.Terminate();
            glfwTerminate();
            AudioDevice::Terminate();
        }

        void ApplicationType::Run() {
            AE_ASSERT(!m_should_close, "Application has not been initialized");

            // First scene
            SceneManager.UpdateActive();

            AE_ASSERT_WARNING(!SceneManager.IsEmpty(), "No scene was present at the begging of game loop");

            // Main loop
            Clock tick_meter;
            m_start_time = ae::Time(glfwGetTime());

            while (!m_should_close && !ae::SceneManager.IsEmpty())
            {
                // Input
                LayerManager.BeginEventHandling();
                glfwPollEvents();
                LayerManager.EndEventHandling();

                // Operations
                SceneManager.GetActiveScene()->Update();

                // Draw if window is visible
                if (ae::Window.GetContextSize() != Vector2i()) {
                    Window.Clear();
                    LayerManager.Draw();
                    Window.Display();
                }

                // Check for scene change
                SceneManager.UpdateActive();

                // Refresh Entities
                if (g_framework_settings.ecs_refresh_entities_each_tick)
                    EntityManager.Refresh();

                // Get Tickrate
                m_ticktime = tick_meter.GetElapsedTime();
                tick_meter.Restart();
            }

            if (m_close_callback)
                m_close_callback();

            Terminate();
        }
        void ApplicationType::Close() {
            m_should_close = true;
        }

        void ApplicationType::SetCloseCallback(const std::function<void()>& callback) { m_close_callback = callback; }

    }

}