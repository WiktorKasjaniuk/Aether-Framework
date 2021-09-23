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

#include "../System/Vector2.hpp"
#include "Color.hpp"

#include <string>

namespace ae {
	
	class TextureCanvas;
	class Framebuffer;
	class Texture;

	bool operator==(const Texture& left, const Texture& right);
	bool operator!=(const Texture& left, const Texture& right);

	class Texture {
		friend class ae::Framebuffer;

		friend bool ae::operator==(const Texture& left, const Texture& right);
		friend bool ae::operator!=(const Texture& left, const Texture& right);

	public:
		Texture() = default;
		Texture(Texture&& move) noexcept;
		Texture(const Texture&) = delete;
		~Texture();

		bool LoadFromFile(const std::string& filename);
		void LoadFromCanvas(const TextureCanvas& canvas);
		void LoadFromData(const Color* pixels, const Vector2ui& size);

		bool WasLoaded() const;

		void Bind(size_t sampler2d_slot) const;
		void Unbind(size_t sampler2d_slot) const;

		void CopyPixelData(Color* output) const;
		const Vector2ui& GetSize() const;

		static size_t GetBindLimit();

	private:
		std::uint32_t m_texture_id = 0;
		Vector2ui m_size;

		void CreateForFramebuffer(const Vector2ui& size);

		void SubmitToOpenGL(void* pixel_data, bool generate_mipmaps = true);
		void Resize(const Vector2ui& new_size, const void* new_pixel_data, bool generate_mipmaps = false);
	};
}