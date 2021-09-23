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


#include "Graphics/Framebuffer.hpp"

#include "Core/OpenGLCalls.hpp"
#include "Core/Preprocessor.hpp"

#include "Structure/Window.hpp"

#include <glad/glad.h>

namespace ae {
	std::uint32_t Framebuffer::s_bound_framebuffer_id = 0;

	Framebuffer::Framebuffer(bool stencil_buffer) {

		// Create Framebuffer
		AE_GL_LOG(glGenFramebuffers(1, &m_framebuffer_id));
		AE_GL_LOG(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id));

		// Create Texture
		m_texture.CreateForFramebuffer(Window.GetContextSize());

		// Create Render Buffer (for stensil)
		m_stencil_present = stencil_buffer;
		if (stencil_buffer) {
			AE_GL_LOG(glGenRenderbuffers(1, &m_renderbuffer_id));
			AE_GL_LOG(glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer_id));
			AE_GL_LOG(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, Window.GetContextSize().x, Window.GetContextSize().y));
			AE_GL_LOG(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_renderbuffer_id));
		}

		Unbind();
	}
	Framebuffer::~Framebuffer() {
		if (m_stencil_present)
			AE_GL_LOG(glDeleteRenderbuffers(1, &m_renderbuffer_id));

		AE_GL_LOG(glDeleteFramebuffers(1, &m_framebuffer_id));
	}

	const Texture& Framebuffer::GetTexture() const {
		return m_texture;
	}
	void Framebuffer::Resize(const Vector2f& new_size) {
		AE_ASSERT(s_bound_framebuffer_id == m_framebuffer_id, "Framebuffer must be bound before resizing");

		m_texture.Resize(new_size, NULL);

		if (new_size.x != 0.f && new_size.y != 0.f) {
			if (m_stencil_present) {
				AE_GL_LOG(glBindRenderbuffer(GL_RENDERBUFFER, m_renderbuffer_id));
				AE_GL_LOG(glRenderbufferStorage(GL_RENDERBUFFER, GL_STENCIL_INDEX8, static_cast<GLsizei>(new_size.x), static_cast<GLsizei>(new_size.y)));
			}

			AE_GL_LOG(glViewport(0, 0, new_size.x, new_size.y));
		}
	}

	void Framebuffer::Bind() const {
		if (s_bound_framebuffer_id == m_framebuffer_id)
			return;

		const Vector2ui& size = m_texture.GetSize();

		if (size.x != 0.f && size.y != 0.f)
			AE_GL_LOG(glViewport(0, 0, size.x, size.y));

		AE_GL_LOG(glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer_id));
		s_bound_framebuffer_id = m_framebuffer_id;
	}
	void Framebuffer::Unbind() const {
		if (s_bound_framebuffer_id == 0)
			return;

		const Vector2i& size = Window.GetContextSize();
		AE_GL_LOG(glViewport(0, 0, size.x, size.y));
		AE_GL_LOG(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		s_bound_framebuffer_id = 0;
	}
	
	void Framebuffer::SetClearColor(const Color& color) {
		m_clear_color = color.GetNormalized();
	}
	void Framebuffer::SetClearStencil(std::uint8_t value) {
		m_clear_stencil = value;
	}
	void Framebuffer::Clear() const {
		AE_ASSERT(s_bound_framebuffer_id == m_framebuffer_id, "Framebuffer must be bound before clearing");

		AE_GL_LOG(glClearColor(m_clear_color.r, m_clear_color.g, m_clear_color.b, m_clear_color.a));

		if (m_stencil_present) {
			AE_GL_LOG(glClearStencil(m_clear_stencil));
			AE_GL_LOG(glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
		}
		else {
			AE_GL_LOG(glClear(GL_COLOR_BUFFER_BIT));
		}
	}
}