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

#include "../Core/CreateStructure.hpp"

#include "../System/Vector2.hpp"
#include "../Graphics/Color.hpp"
#include "../Graphics/TextureCanvas.hpp"

#include <string>

namespace ae {

	struct ContextSettings {
		unsigned int opengl_version_major = 4;
		unsigned int opengl_version_minor = 6;

		bool default_framebuffer_stencil_buffer = false;
		unsigned int default_framebuffer_antialiasing_samples = 0;
	};
		
	namespace internal {

		class ApplicationType;
		void* GetNativeWindow();

		class WindowType {
			friend class ae::internal::ApplicationType;
			friend void* ae::internal::GetNativeWindow();

			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<WindowType>();

		public:
			~WindowType() = default;

			void Recreate(const ContextSettings& context_settings);

			void SetClearColor(const Color& color);
			void SetClearStencil(unsigned char value);

			void SetTitle(const std::string& title);
			const std::string& GetTitle() const;

			void SetIcon(const ae::TextureCanvas& icon);
			void SetIcon(const ae::TextureCanvas& icon16x16, const ae::TextureCanvas& icon32x32, const ae::TextureCanvas& icon48x48);

			void Resize(const Vector2i& size);
			Vector2i GetContextSize() const;
			Vector2i GetPosition() const;

			void GoFullScreen(int monitor);
			void GoWindowed();
			bool IsFullScreen();
			int GetMonitorCount();

			void Focus();
			bool HasFocus() const;

			void SetVisible(bool should_be_visible);

			void RequestAttention();

			void SetVSync(bool vsync);

		private:
			void* m_native_window = NULL;

			std::string m_title;
			Vector4f m_clear_color;
			unsigned char m_clear_stencil = 0;

			WindowType() = default;
			WindowType(const WindowType&) = delete;
			WindowType(WindowType&&) = delete;

			void Initialize(const Vector2i& size, const std::string& title, const ContextSettings& context_settings);
			void Terminate();

			void Clear();
			void Display();
		};
	}

	extern ae::internal::WindowType Window;

}