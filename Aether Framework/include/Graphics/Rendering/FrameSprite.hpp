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

//////////////////////////////////////////
/// FrameSprite
///
/// Framebuffer's counterpart of Sprite. If framebuffer was not set it doesn't draw.
/// 
/// Vertex Coordinates:
///		> vertex positions are in pixels, so SetSize(framebuffer_size) should be called whenever framebuffer's size changes,
///		> vertex coordinates are in normalized space, so SetTextureRect() should also be.
/// 
/// Used Shaders:
/// 
///		Vertex Shader:
///			version 460 core 
///			
///			layout(location = 0) in vec2 a_position; 
///			layout(location = 1) in vec2 a_texcoords; 
///			
///			out vec2 v_texcoords; 
///			
///			uniform mat3 u_mvp; 
///			
///			void main() 
///			{ 
///				gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0); 
///				v_texcoords = a_texcoords; 
///			}
///		
///		Fragment Shader:
///			#version 460 core 
///			
///			layout(location = 0) out vec4 a_color; 
///			
///			in vec2 v_texcoords; 
///			
///			uniform sampler2D u_texture; 
///			uniform vec4 u_color; 
///			
///			void main() 
///			{ 
///				// output color 
///				a_color = u_color * texture(u_texture, v_texcoords); 
///			}
/// 
//////////////////////////////////////////
#pragma once

#include "VertexArray.hpp"
#include "../Texture.hpp"
#include "../Shader.hpp"
#include "../../System/Vector2.hpp"
#include "../Matrix3x3.hpp"
#include "../Color.hpp"
#include "../Framebuffer.hpp"
#include "../../Structure/Camera.hpp"

#include <functional>

namespace ae {
	
	class FrameSprite {
	public:
		FrameSprite(const Vector2f& size = Vector2f());
		FrameSprite(const FrameSprite&) = default;
		FrameSprite(FrameSprite&&) = default;

		FrameSprite& operator=(const FrameSprite&) = default;
		FrameSprite& operator=(FrameSprite&&) = default;

		void Draw(const Matrix3x3& transform = Camera.GetProjMatrix()) const;
		void Draw(const Shader& shader, const Matrix3x3& transform = Camera.GetProjMatrix()) const;
		void Draw(const std::function<void(const Texture*, const Color&, const VertexArray<VertexPosTex>&)>& draw) const;

		template <typename ...HandlerTypes>
		void Draw(const std::function<void(const Texture*, const Color&, const VertexArray<VertexPosTex>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const;
		
		void SetColor(const Color& color);
		const Color& GetColor() const;

		void SetFramebuffer(const Framebuffer& frame_buffer);

		void SetTextureRect(const FloatRect& rect);
		const FloatRect& GetTextureRect() const;

		void SetSize(const Vector2f& new_size);
		const Vector2f& GetSize() const;

	private:
		mutable VertexArray<VertexPosTex> m_vertices;
		const Texture* m_texture = nullptr;
		Color m_color;
		Vector2f m_size;
		FloatRect m_texture_rect = FloatRect(0.f, 0.f, 1.f, 1.f);
	};

	// Previous template definitions
	template <typename ...HandlerTypes>
	void FrameSprite::Draw(const std::function<void(const Texture*, const Color&, const VertexArray<VertexPosTex>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const {
		draw(m_texture, m_color, m_vertices, handlers...);
	}
}