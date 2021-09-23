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

#include "Structure/Clipboard.hpp"
#include "Structure/Window.hpp"

#include <glfw/glfw3.h>
#include <iostream>

namespace ae {
	internal::ClipboardType Clipboard = internal::CreateStructure<internal::ClipboardType>();

	namespace internal {

		void ClipboardType::SetSystemStorage(const std::string& string) {
			GLFWwindow* window = static_cast<GLFWwindow*>(ae::internal::GetNativeWindow());
			glfwSetClipboardString(window, string.c_str());
		}
		std::string ClipboardType::GetSystemStorage() const {
			GLFWwindow* window = static_cast<GLFWwindow*>(ae::internal::GetNativeWindow());
			return glfwGetClipboardString(window);
		}

		void ClipboardType::SetCustomStorage(int storage, const std::string& string) {

			// Find
			auto storage_it = m_storage.find(storage);

			// Set string
			if (storage_it == m_storage.end())
				m_storage.emplace(storage, string);
			else
				storage_it->second = string;
		}
		const std::string& ClipboardType::GetCustomStorage(int storage) const {

			// Find
			auto storage_it = m_storage.find(storage);

			// Return string
			if (storage_it == m_storage.end())
				return m_default_string;
			else
				return storage_it->second;
		}
	}
}