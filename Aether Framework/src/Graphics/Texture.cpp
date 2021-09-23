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


#include "Graphics/Texture.hpp"
#include "Graphics/TextureCanvas.hpp"

#include "Core/Preprocessor.hpp"
#include "Core/OpenGLCalls.hpp"
#include "System/LogError.hpp"

#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image/stb_image.h>

#include <iostream>
#include <string>

namespace ae {
	Texture::Texture(Texture&& move) noexcept {
		m_texture_id = move.m_texture_id;
		m_size = std::move(move.m_size);
		move.m_texture_id = 0;
	}
	Texture::~Texture() {
		AE_GL_LOG(glDeleteTextures(1, &m_texture_id));
	}

	bool Texture::WasLoaded() const { return (m_texture_id != 0); }

	bool Texture::LoadFromFile(const std::string& filename) {

		AE_ASSERT(!WasLoaded(), "Texture was already loaded");

		// Create
		AE_GL_LOG(glGenTextures(1, &m_texture_id));

		// Load File
		int width, height, channels;

		void* pixel_data = stbi_load(filename.c_str(), &width, &height, &channels, 4);
		m_size.x = width;
		m_size.y = height;
	
		// Success
		if (pixel_data) {
			SubmitToOpenGL(pixel_data);
			delete[] pixel_data;
		}
		else {
			AE_GL_LOG(glDeleteTextures(1, &m_texture_id));
			m_size = {};
			m_texture_id = 0;
		}

		// Return
		return (pixel_data != nullptr);
	}
	void Texture::LoadFromCanvas(const TextureCanvas& canvas) {
		LoadFromData(canvas.GetPixelData(), canvas.GetSize());
	}
	void Texture::LoadFromData(const Color* pixels, const Vector2ui& size) {

		AE_ASSERT(!WasLoaded(), "Texture was already loaded");

		// Create
		AE_GL_LOG(glGenTextures(1, &m_texture_id));

		void* pixel_data = new Color[size.x * size.y];
		memcpy(pixel_data, pixels, size.x * size.y * 4);

		m_size = size;
		SubmitToOpenGL(pixel_data);

		delete[] pixel_data;
	}
	void Texture::CreateForFramebuffer(const Vector2ui& size) {
		AE_GL_LOG(glGenTextures(1, &m_texture_id));
		m_size = size;
		SubmitToOpenGL(NULL, false);

		AE_GL_LOG(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_texture_id, 0));
	}
	void Texture::SubmitToOpenGL(void* pixel_data, bool generate_mipmaps) {

		// Bind
		Bind(0);

		// Store
		AE_GL_LOG(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		AE_GL_LOG(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel_data));

		// Generate Mipmaps
		if (generate_mipmaps) 
			AE_GL_LOG(glGenerateMipmap(GL_TEXTURE_2D));
		
		// Set Parameters
		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}
	void Texture::Resize(const Vector2ui& new_size, const void* new_pixel_data, bool generate_mipmaps) {

		Bind(0);
		m_size = new_size;

		// New Storage
		AE_GL_LOG(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_size.x, m_size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, new_pixel_data));

		// Generate Mipmaps
		if (generate_mipmaps)
			AE_GL_LOG(glGenerateMipmap(GL_TEXTURE_2D));
	}

	void Texture::Bind(size_t sampler2d_slot) const {
		if (!WasLoaded()) {
			AE_ASSERT_FALSE("Could not bind texture, it has not been loaded yet");
			LogError("Could not bind texture, it has not been loaded yet. Perhaps the file missing?", true);
			std::exit(EXIT_FAILURE);
		}
		AE_ASSERT(sampler2d_slot < GetBindLimit(), "Could not bind texture to slot '" << sampler2d_slot << "' it exceeds bind limit '" << GetBindLimit() << '\'');
		AE_GL_LOG(glActiveTexture(GL_TEXTURE0 + static_cast<GLint>(sampler2d_slot)));
		AE_GL_LOG(glBindTexture(GL_TEXTURE_2D, m_texture_id));
	}
	void Texture::Unbind(size_t sampler2d_slot) const {
		AE_ASSERT(sampler2d_slot < GetBindLimit(), "Could not unbind texture from slot '" << sampler2d_slot << "' it exceeds bind limit '" << GetBindLimit() << '\'');
		AE_GL_LOG(glActiveTexture(GL_TEXTURE0 + static_cast<GLint>(sampler2d_slot)));
		AE_GL_LOG(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void Texture::CopyPixelData(Color* output) const {
		AE_ASSERT(output, "Could not copy texture to nullptr");

		if (m_texture_id == 0){
			output = nullptr;
			AE_WARNING("Could not copy texture, it has not beeen loaded yet, returning nullptr");
			return;
		}

		Bind(0);

		output = new Color[m_size.x * m_size.y];
		AE_GL_LOG(glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, output));
	}
	const Vector2ui& Texture::GetSize() const { return m_size; }

	size_t Texture::GetBindLimit() {
		GLint value;
		AE_GL_LOG(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &value));
		return static_cast<size_t>(value);
	}


	bool operator==(const Texture& left, const Texture& right) {
		return (left.m_texture_id == right.m_texture_id);
	}
	bool operator!=(const Texture& left, const Texture& right) {
		return (left.m_texture_id != right.m_texture_id);
	}

}