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


#include "Graphics/Rendering/Sprite.hpp"
#include "Core/Preprocessor.hpp"
#include "Structure/AssetManager.hpp"

namespace ae {

	Sprite::Sprite(const Vector2f& size) {
		m_size = size;
		m_vertices.Append(
			{
				VertexPosTex( Vector2f(0.f,    0.f   ), Vector2f() ),
				VertexPosTex( Vector2f(size.x, 0.f   ), Vector2f() ),
				VertexPosTex( Vector2f(size.x, size.y), Vector2f() ),
				VertexPosTex( Vector2f(0.f,    size.y), Vector2f() )
			},
			{
				0, 1, 2,
				2, 3, 0
			}
		);

		m_vertices.Bind();
		m_vertices.AddLayout<Vector2f>(0, offsetof(VertexPosTex, position), false);
		m_vertices.AddLayout<Vector2f>(1, offsetof(VertexPosTex, texcoords), false);

	}
	void Sprite::Draw(const Matrix3x3& transform) const {
		Draw(DefaultAssets->sprite_shader, transform);
	}
	void Sprite::Draw(const Shader& shader, const Matrix3x3& transform) const {

		shader.Bind();

		// Set texture
		if (m_texture)
			m_texture->Bind(0);
		else
			DefaultAssets->white_pixel_texture.Bind(0);

		// Set uniforms
		shader.SetUniform.Sampler2D("u_texture", 0);
		shader.SetUniform.Vec4f("u_color", m_color.GetNormalized());
		shader.SetUniform.Mat3x3("u_mvp", transform.GetArray());

		// Draw
		m_vertices.Bind();
		m_vertices.Draw();
	}
	void Sprite::Draw(const std::function<void(const Texture*, const Color&, const VertexArray<VertexPosTex>&)>& draw) const {
		draw(m_texture, m_color, m_vertices);
	}

	void Sprite::SetColor(const Color& color) { m_color = color; }
	const Vector2f& Sprite::GetSize() const { return m_size; }
	const Color& Sprite::GetColor() const { return m_color; }
	const IntRect& Sprite::GetTextureRect() const { return m_texture_rect; }
	const Texture* Sprite::GetTexture() const { return m_texture; }

	void Sprite::SetTextureRect(const IntRect& rect) { 
		m_texture_rect = rect; 
		UpdateTextureCoords();
	}
	void Sprite::SetTexture(const Texture& texture) {
		if (!texture.WasLoaded())
			return;

		if (!m_texture) {
			if (m_texture_rect == IntRect())
				SetTextureRect(IntRect(Vector2i(0, 0), texture.GetSize()));
			if (m_size == Vector2f())
				SetSize(texture.GetSize());
		}

		m_texture = &texture;
	}
	void Sprite::SetSize(const Vector2f& new_size) {
		m_size = new_size;

		std::vector<VertexPosTex> new_vertices = m_vertices.GetVertices();

		new_vertices[1].position = Vector2f(m_size.x, 0.f);
		new_vertices[2].position = Vector2f(m_size.x, m_size.y);
		new_vertices[3].position = Vector2f(0.f, m_size.y);

		m_vertices.SetVertices(new_vertices);
	}
	void Sprite::UpdateTextureCoords() {
		std::vector<VertexPosTex> new_vertices = m_vertices.GetVertices();

		new_vertices[0].texcoords = Vector2f(m_texture_rect.left,                        m_texture_rect.top                        );
		new_vertices[1].texcoords = Vector2f(m_texture_rect.left + m_texture_rect.width, m_texture_rect.top                        );
		new_vertices[2].texcoords = Vector2f(m_texture_rect.left + m_texture_rect.width, m_texture_rect.top + m_texture_rect.height);
		new_vertices[3].texcoords = Vector2f(m_texture_rect.left,                        m_texture_rect.top + m_texture_rect.height);

		m_vertices.SetVertices(new_vertices);
	}

}