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

#include "Core/Preprocessor.hpp"

#include "Structure/Cursor.hpp"
#include "Structure/Window.hpp"

#include <glfw/glfw3.h>

namespace ae {

	internal::CursorType Cursor = internal::CreateStructure<internal::CursorType>();

	namespace internal {

		CursorType::CursorType() : m_current_mode(CursorMode::Arrow) {}

		void CursorType::Initialize() {
			for (int cursor_mode(GLFW_ARROW_CURSOR); cursor_mode <= GLFW_VRESIZE_CURSOR; cursor_mode++) {

				GLFWcursor* cursor = glfwCreateStandardCursor(cursor_mode);
				m_cursors.push_back(cursor);
			}
		}
		void CursorType::Terminate() {
			while (!m_cursors.empty()) {
				glfwDestroyCursor(static_cast<GLFWcursor*>(m_cursors.back()));
				m_cursors.pop_back();
			}
		}

		void CursorType::Disable() {
			glfwSetInputMode(static_cast<GLFWwindow*>(GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		void CursorType::Hide() {
			glfwSetInputMode(static_cast<GLFWwindow*>(GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		}
		void CursorType::Show() {
			glfwSetInputMode(static_cast<GLFWwindow*>(GetNativeWindow()), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}

		void CursorType::SetMode(CursorMode mode) {
			AE_ASSERT(static_cast<size_t>(mode) < m_cursors.size(), "No such cursor mode has been uploaded");
			glfwSetCursor(static_cast<GLFWwindow*>(GetNativeWindow()), static_cast<GLFWcursor*>(m_cursors.at(static_cast<size_t>(mode))));
			m_current_mode = mode;
		}
		CursorMode CursorType::GetCurrentMode() const {
			return m_current_mode;
		}

		void CursorType::UploadDefaultIcon(CursorMode mode, const TextureCanvas& icon, const Vector2i& hot_spot) {
			AE_ASSERT(mode < CursorMode::Custom0, "'mode' must be one of the default cursor modes!");

			GLFWimage image;
			image.width  = icon.GetSize().x;
			image.height = icon.GetSize().y;

			image.pixels = new unsigned char[image.width * image.height * 4];
			memcpy(image.pixels, icon.GetPixelData(), image.width * image.height * 4);

			glfwDestroyCursor(static_cast<GLFWcursor*>(m_cursors.at(static_cast<size_t>(mode))));
			m_cursors.at(static_cast<size_t>(mode)) = glfwCreateCursor(&image, hot_spot.x, hot_spot.y);

			delete[] image.pixels;

			if (m_current_mode == mode)
				glfwSetCursor(static_cast<GLFWwindow*>(GetNativeWindow()), static_cast<GLFWcursor*>(m_cursors.at(static_cast<size_t>(mode))));
		}
		CursorMode CursorType::UploadCustomIcon(const TextureCanvas& icon, const Vector2i& hot_spot) {

			GLFWimage image;
			image.width = icon.GetSize().x;
			image.height = icon.GetSize().y;

			image.pixels = new unsigned char[image.width * image.height * 4];
			memcpy(image.pixels, icon.GetPixelData(), image.width * image.height * 4);

			GLFWcursor* cursor = glfwCreateCursor(&image, hot_spot.x, hot_spot.y);
			m_cursors.push_back(cursor);

			delete[] image.pixels;

			return static_cast<CursorMode>(m_cursors.size() - 1);
		}
	}
}