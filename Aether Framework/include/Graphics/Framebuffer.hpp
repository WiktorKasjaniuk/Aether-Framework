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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Framebuffer
///
/// Class storing frame fragments.
/// To enable stencil testing construct the framebuffer with true.
/// 
/// GetTexture() returns special texture which changes size if Resize() function is called.
/// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Graphics/Texture.hpp"
#include "../Graphics/Shader.hpp"

#include <cstdint>

namespace ae {
	class Framebuffer {
	public:
		Framebuffer(bool stencil_buffer = false);
		Framebuffer(const Framebuffer&) = delete;
		Framebuffer(Framebuffer&&) = delete;
		~Framebuffer();

		const Texture& GetTexture() const;

		void Bind() const;
		void Unbind() const;

		void Resize(const Vector2f& new_size);

		void SetClearColor(const Color& color);
		void SetClearStencil(std::uint8_t value);
		void Clear() const;

	private:
		Texture m_texture;
		std::uint32_t m_framebuffer_id;
		std::uint32_t m_renderbuffer_id;
		static std::uint32_t s_bound_framebuffer_id;

		Vector4f m_clear_color = ae::Color::Cavern.GetNormalized();
		std::uint8_t m_clear_stencil = 0;
		bool m_stencil_present;
	};
}