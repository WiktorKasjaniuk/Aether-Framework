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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Sprite
///
/// General Idea:
///		Sprite is a textured and colored quad renderer.
/// 
///	Parameters:
///		Color:
///			Either quad's color or the color each texture's pixel is multiplied with. It's an uniform.
/// 
///		Texture:
///			Sprite's texture.
/// 
///		Texture Rectangle:
///			Texture's sector / pixel position and size in texture, which is fit into sprite's size when drawing.
///			When setting texture first time, and texture rect was not set before, it becomes IntRect({0, 0}, texture size).
/// 
///		Size:
///			Sprite's pixel size, do not confuse with texture size (texture rectangle).
///			When setting texture first time, and size was not set before, it becomes the texture size.
/// 
/// Drawing:
///		To draw sprite just pass transform parameter, also a shader if default shaders listed below are not suitable.
///		It is also possible to pass custom draw function along with it's handlers, example:
///			float factor = 1.5f;		
/// 
///			auto foo = [](const Texture* texture, const Color& color, const VertexArray<VertexPosTex>&, float& factor) {
///				color *= factor;
///			
///				// ... 
///				// do some drawing
///			}
/// 
///			sprite.Draw<float>(foo, factor);
/// 
///		If texture was not set, 1 px white texture will be used instead.
/// 
/// Used Shaders:
///		Vertex Shader:
///			#version 460 core
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
///			in vec4 v_color;
///			in vec2 v_texcoords;
///			
///			uniform sampler2D u_texture;
///			uniform vec4 u_color;
///			
///			void main()
///			{
///				// normalize pixel texture coords
///				vec2 normalized_coords = v_texcoords / vec2(textureSize(u_texture, 0));
///			
///				// output color
///				a_color = u_color * texture(u_texture, normalized_coords);
///			}
///			
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "VertexArray.hpp"
#include "../Texture.hpp"
#include "../Shader.hpp"
#include "../../System/Vector2.hpp"
#include "../Matrix3x3.hpp"
#include "../Color.hpp"
#include "../../Structure/Camera.hpp"

#include <functional>

namespace ae {

	class Sprite {
	public:
		Sprite(const Vector2f& size = Vector2f());
		Sprite(const Sprite&) = default;
		Sprite(Sprite&&) = default;

		Sprite& operator=(const Sprite&) = default;
		Sprite& operator=(Sprite&&) = default;

		void Draw(const Matrix3x3& transform = Camera.GetProjMatrix()) const;
		void Draw(const Shader& shader, const Matrix3x3& transform = Camera.GetProjMatrix()) const;
		void Draw(const std::function<void(const Texture*, const Color&, const VertexArray<VertexPosTex>&)>& draw) const;

		template <typename ...HandlerTypes>
		void Draw(const std::function<void(const Texture*, const Color&, const VertexArray<VertexPosTex>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const;

		void SetColor(const Color& color);
		const Color& GetColor() const;

		void SetTexture(const Texture& texture);
		const Texture* GetTexture() const;

		void SetTextureRect(const IntRect& rect);
		const IntRect& GetTextureRect() const;

		void SetSize(const Vector2f& new_size);
		const Vector2f& GetSize() const;

	private:
		VertexArray<VertexPosTex> m_vertices;
		const Texture* m_texture = nullptr;
		Color m_color;
		Vector2f m_size;
		IntRect m_texture_rect;

		void UpdateTextureCoords();
	};

	// Previous template definitions
	template <typename ...HandlerTypes>
	void Sprite::Draw(const std::function<void(const Texture*, const Color&, const VertexArray<VertexPosTex>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const {
		draw(m_texture, m_color, m_vertices, handlers...);
	}
}
