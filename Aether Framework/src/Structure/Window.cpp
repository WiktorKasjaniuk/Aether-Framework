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
#include "Core/OpenGLCalls.hpp"

#include "Structure/Window.hpp"

#include <glfw/glfw3.h>

namespace ae {

	internal::WindowType Window = internal::CreateStructure<internal::WindowType>();

	namespace internal {

		void* GetNativeWindow() {
			return ae::Window.m_native_window;
		}

		void WindowType::Terminate() {
			glfwDestroyWindow(static_cast<GLFWwindow*>(m_native_window));
		}

		void WindowType::Initialize(const Vector2i& size, const std::string& title, const ContextSettings& context_settings) {
#ifdef AE_DEBUG
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#else
			glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, false);
#endif
			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, context_settings.opengl_version_major);
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, context_settings.opengl_version_minor);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
			glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE);
			glfwWindowHint(GLFW_DEPTH_BITS, 0);
			glfwWindowHint(GLFW_STENCIL_BITS, context_settings.default_framebuffer_stencil_buffer * 8);
			glfwWindowHint(GLFW_SAMPLES, context_settings.default_framebuffer_antialiasing_samples);

			GLFWwindow* glfw_window = glfwCreateWindow(size.x, size.y, title.c_str(), NULL, NULL);
			m_native_window = glfw_window;

			AE_ASSERT(m_native_window, "Could not create a GLFW window")

			glfwMakeContextCurrent(glfw_window);

			m_title = title;

			glfwSetFramebufferSizeCallback(glfw_window,
				[](GLFWwindow* window, int width, int height) {
					glViewport(0, 0, width, height);
			});

		}

		void WindowType::Recreate(const ContextSettings& context_settings) {
			Vector2i size = GetContextSize();
			Terminate();
			Initialize(size, m_title, context_settings);
		}

		void WindowType::SetClearColor(const Color& color) {
			m_clear_color = color.GetNormalized();
		}
		void WindowType::SetClearStencil(unsigned char value) {
			m_clear_stencil = value;
		}

		void WindowType::Clear() {
			AE_GL_LOG(glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a));
			AE_GL_LOG(glClearStencil(m_clear_stencil));

			AE_GL_LOG(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		}
		void WindowType::Display() {
			glfwSwapBuffers(static_cast<GLFWwindow*>(m_native_window));
		}

		void WindowType::SetTitle(const std::string& title) {
			glfwSetWindowTitle(static_cast<GLFWwindow*>(m_native_window), title.c_str());
			m_title = title;
		}
		const std::string& WindowType::GetTitle() const {
			return m_title;
		}
		void WindowType::SetIcon(const ae::TextureCanvas& icon) {

			GLFWimage image;
			image.width  = icon.GetSize().x;
			image.height = icon.GetSize().y;

			image.pixels = new unsigned char[image.width * image.height * 4];
			memcpy(image.pixels, icon.GetPixelData(), image.width * image.height * 4);

			glfwSetWindowIcon(static_cast<GLFWwindow*>(m_native_window), 1, &image);

			delete[] image.pixels;
		}
		void WindowType::SetIcon(const ae::TextureCanvas& icon16x16, const ae::TextureCanvas& icon32x32, const ae::TextureCanvas& icon48x48) {
			
			GLFWimage images[3];
			
			// 16x16
			images[0].width  = icon16x16.GetSize().x;
			images[0].height = icon16x16.GetSize().y;

			images[0].pixels = new unsigned char[images[0].width * images[0].height * 4];
			memcpy(images[0].pixels, icon16x16.GetPixelData(), images[0].width * images[0].height * 4);

			// 32x32
			images[1].width = icon32x32.GetSize().x;
			images[1].height = icon32x32.GetSize().y;

			images[1].pixels = new unsigned char[images[1].width * images[1].height * 4];
			memcpy(images[1].pixels, icon32x32.GetPixelData(), images[1].width * images[1].height * 4);

			// 48x48
			images[2].width = icon48x48.GetSize().x;
			images[2].height = icon48x48.GetSize().y;

			images[2].pixels = new unsigned char[images[2].width * images[2].height * 4];
			memcpy(images[2].pixels, icon48x48.GetPixelData(), images[2].width * images[2].height * 4);

			// upload
			glfwSetWindowIcon(static_cast<GLFWwindow*>(m_native_window), 1, images);

			delete[] images[0].pixels;
			delete[] images[1].pixels;
			delete[] images[2].pixels;
		}

		void WindowType::Resize(const Vector2i& size) {
			glfwSetWindowSize(static_cast<GLFWwindow*>(m_native_window), size.x, size.y);
		}
		Vector2i WindowType::GetContextSize() const {
			Vector2i size;
			glfwGetWindowSize(static_cast<GLFWwindow*>(m_native_window), &size.x, &size.y);
			return size;
		}
		Vector2i WindowType::GetPosition() const {
			Vector2i position;
			glfwGetWindowPos(static_cast<GLFWwindow*>(m_native_window), &position.x, &position.y);
			return position;
		}

		int WindowType::GetMonitorCount() {
			int count;
			glfwGetMonitors(&count);
			return count;
		}
		void WindowType::GoFullScreen(int monitor = 0) {
			if (IsFullScreen())
				return;

			int count;
			GLFWmonitor** monitors = glfwGetMonitors(&count);


			if ((int)monitor < count) {
				GLFWmonitor* monitor_object = monitors[monitor];
				const GLFWvidmode* video_mode = glfwGetVideoMode(monitor_object);

				glfwSetWindowMonitor(static_cast<GLFWwindow*>(m_native_window), monitor_object, 0, 0, video_mode->width, video_mode->height, video_mode->refreshRate);
			}

		}
		void WindowType::GoWindowed() {
			if (!IsFullScreen())
				return;

			Vector2i size = GetContextSize();

			glfwSetWindowMonitor(static_cast<GLFWwindow*>(m_native_window), 0, 100, 100, size.x, size.y, GLFW_DONT_CARE);
		}
		bool WindowType::IsFullScreen() {
			return (glfwGetWindowMonitor(static_cast<GLFWwindow*>(m_native_window)) != 0);
		}

		void WindowType::Focus() {
			glfwFocusWindow(static_cast<GLFWwindow*>(m_native_window));
		}
		bool WindowType::HasFocus() const {
			return glfwGetWindowAttrib(static_cast<GLFWwindow*>(m_native_window), GLFW_FOCUSED);
		}

		void WindowType::SetVisible(bool should_be_visible) {
			if (should_be_visible)
				glfwShowWindow(static_cast<GLFWwindow*>(m_native_window));
			else
				glfwHideWindow(static_cast<GLFWwindow*>(m_native_window));
		}

		void WindowType::RequestAttention() {
			glfwRequestWindowAttention(static_cast<GLFWwindow*>(m_native_window));
		}

		void WindowType::SetVSync(bool vsync) {
			glfwSwapInterval(vsync);
		}

	}

}