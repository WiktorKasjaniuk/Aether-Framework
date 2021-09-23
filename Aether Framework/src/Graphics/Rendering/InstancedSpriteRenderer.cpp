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


#include "Graphics/Rendering/InstancedSpriteRenderer.hpp"
#include "Structure/AssetManager.hpp"

namespace ae {

	InstancedSpriteRenderer::InstancedSpriteRenderer() {
		m_vertices.Append(
			{
				VertexPosTex(Vector2f(), Vector2f()),
				VertexPosTex(Vector2f(), Vector2f()),
				VertexPosTex(Vector2f(), Vector2f()),
				VertexPosTex(Vector2f(), Vector2f())
			},
			{
				0, 1, 2, 2, 3, 0
			}
			);

		m_vertices.Bind();
		m_vertices.AddLayout<Vector2f>(0, offsetof(VertexPosTex, position), false);
		m_vertices.AddLayout<Vector2f>(1, offsetof(VertexPosTex, texcoords), false);
	}
	InstancedSpriteRenderer::InstancedSpriteRenderer(const InstancedSpriteRenderer& copy) {
		*this = copy;
	}
	InstancedSpriteRenderer::InstancedSpriteRenderer(InstancedSpriteRenderer&& move) noexcept {
		*this = std::move(move);
	}

	InstancedSpriteRenderer& InstancedSpriteRenderer::operator=(const InstancedSpriteRenderer& copy) {

		// copy renderer data
		m_vertices = copy.m_vertices;
		m_size = copy.m_size;
		m_texture = copy.m_texture;
		m_texture_rect = copy.m_texture_rect;

		// copy uniform data
		m_transforms = copy.m_transforms;
		m_colors = copy.m_colors;

		// copy instances' data
		m_instances.reserve(copy.m_instances.size());

		for (auto& instance_uptr : copy.m_instances) {
			InstancedSprite* instance = new InstancedSprite();
			instance->m_renderer = this;
			instance->m_index = instance_uptr.get()->m_index;

			m_instances.emplace_back(instance);
		}

		return *this;
	}
	InstancedSpriteRenderer& InstancedSpriteRenderer::operator=(InstancedSpriteRenderer&& move) noexcept {

		// move renderer data
		m_vertices = std::move(move.m_vertices);
		m_size = std::move(move.m_size);
		m_texture = std::move(move.m_texture);
		m_texture_rect = std::move(move.m_texture_rect);

		// move uniform data
		m_transforms = std::move(move.m_transforms);
		m_colors = std::move(move.m_colors);

		// move instances' data
		m_instances = std::move(move.m_instances);

		for (auto& instance_uptr : m_instances)
			instance_uptr->m_renderer = this;

		return *this;
	}

	void InstancedSpriteRenderer::Draw(const Matrix3x3& transform) const {
		Draw(DefaultAssets->instanced_sprite_shader, transform);
	}
	void InstancedSpriteRenderer::Draw(const Shader& shader, const Matrix3x3& transform) const {

		shader.Bind();
		shader.SetUniform.Mat3x3("u_vp", transform.GetArray());

		if (m_texture)
			m_texture->Bind(0);
		else
			DefaultAssets->white_pixel_texture.Bind(0);

		shader.SetUniform.Sampler2D("u_texture", 0);

		m_vertices.Bind();
		size_t max_draws_per_call = DefaultAssets->instanced_sprite_max_draws_per_call;

		for (size_t drawn_instances(0); drawn_instances < GetCount(); drawn_instances += max_draws_per_call) {
			size_t next_draw_count = std::min(max_draws_per_call, GetCount() - drawn_instances);

			shader.SetUniform.Mat3x3("u_models", m_transforms.at(drawn_instances).GetArray(), 0, next_draw_count);

			const Vector4f* colors_data = &m_colors.at(drawn_instances);
			shader.SetUniform.Vec4fArray("u_colors", colors_data, 0, next_draw_count);

			m_vertices.DrawInstanced(next_draw_count);
		}
	}
	void InstancedSpriteRenderer::Draw(const std::function<void(const Texture*, size_t instance_count, const std::vector<Matrix3x3>&, const std::vector<Vector4f>&, const VertexArray<VertexPosTex>&)>& draw) const {
		draw(m_texture, GetCount(), m_transforms, m_colors, m_vertices);
	}

	void InstancedSpriteRenderer::Sort(const std::function<bool(const InstancedSprite& left, const InstancedSprite& right)>& compare) {

		// insertion sort instances
		for (int marker = 1; marker < m_instances.size(); marker++) {

			std::unique_ptr<InstancedSprite> instance = std::move(m_instances[marker]);
			int sub_marker = marker - 1;

			while (sub_marker >= 0 && compare(*m_instances[sub_marker], *instance)) {
				m_instances[sub_marker + 1] = std::move(m_instances[sub_marker]);

				sub_marker--;
			}
			m_instances[sub_marker + 1] = std::move(instance);
		}

		// create new order and update instances' m_index
		std::vector<size_t> new_order;
		new_order.reserve(GetCount());
		for (size_t i(0); i < GetCount(); i++) {
			InstancedSprite& instance = *m_instances.at(i);

			new_order.push_back(instance.m_index);
			instance.m_index = i;
		}

		// safely reorder uniforms by new order
		for (size_t i = 0; i < GetCount() - 1; i++) {
			if (new_order[i] == i)
				continue;

			size_t o;
			for (o = i + 1; o < new_order.size(); ++o)
				if (new_order[o] == i)
					break;

			std::swap(m_transforms[i], m_transforms[new_order[i]]);
			std::swap(m_colors[i], m_colors[new_order[i]]);
			std::swap(new_order[i], new_order[o]);
		}
	}

	void InstancedSpriteRenderer::SetTexture(const Texture& texture) {
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
	void InstancedSpriteRenderer::SetSize(const Vector2f& size) {
		m_size = size;

		std::vector<VertexPosTex> new_vertices = m_vertices.GetVertices();

		new_vertices[1].position = Vector2f(m_size.x, 0.f);
		new_vertices[2].position = Vector2f(m_size.x, m_size.y);
		new_vertices[3].position = Vector2f(0.f, m_size.y);

		m_vertices.SetVertices(new_vertices);
	}
	void InstancedSpriteRenderer::SetTextureRect(const IntRect& rect) {
		m_texture_rect = rect;
		UpdateTextureCoords();
	}
	void InstancedSpriteRenderer::UpdateTextureCoords() {
		std::vector<VertexPosTex> new_vertices = m_vertices.GetVertices();

		new_vertices[0].texcoords = Vector2f(m_texture_rect.left, m_texture_rect.top);
		new_vertices[1].texcoords = Vector2f(m_texture_rect.left + m_texture_rect.width, m_texture_rect.top);
		new_vertices[2].texcoords = Vector2f(m_texture_rect.left + m_texture_rect.width, m_texture_rect.top + m_texture_rect.height);
		new_vertices[3].texcoords = Vector2f(m_texture_rect.left, m_texture_rect.top + m_texture_rect.height);

		m_vertices.SetVertices(new_vertices);
	}
	size_t InstancedSpriteRenderer::GetCount() const {
		return m_instances.size();
	}

	InstancedSprite& InstancedSpriteRenderer::CreateBack(const Matrix3x3& transform, const Color& color) {
		InstancedSprite* instance = m_instances.emplace(m_instances.end(), new InstancedSprite())->get();

		instance->m_renderer = this;
		instance->m_index = GetCount() - 1;

		m_transforms.emplace(m_transforms.end(), transform);
		m_colors.emplace(m_colors.end(), color.GetNormalized());

		return *instance;
	}
	InstancedSprite& InstancedSpriteRenderer::Create(size_t index, const Matrix3x3& transform, const Color& color) {
		InstancedSprite* instance = m_instances.emplace(m_instances.begin() + index, new InstancedSprite())->get();

		instance->m_renderer = this;
		instance->m_index = index;

		m_transforms.emplace(m_transforms.begin() + index, transform);
		m_colors.emplace(m_colors.begin() + index, color.GetNormalized());

		// update other instances' positions
		for (size_t i(index + 1); i < GetCount(); i++)
			m_instances.at(i)->m_index++;

		return *instance;
	}
	void InstancedSpriteRenderer::Destroy(size_t index) {
		m_transforms.erase(m_transforms.begin() + index);
		m_colors.erase(m_colors.begin() + index);
		m_instances.erase(m_instances.begin() + index);

		// update other instances' positions
		for (size_t i(index); i < GetCount(); i++)
			m_instances.at(i)->m_index--;
	}
	void InstancedSpriteRenderer::Destroy(InstancedSprite* instance) {
		Destroy(instance->m_index);
	}
	void InstancedSpriteRenderer::Clear() {
		m_instances.clear();
		m_transforms.clear();
		m_colors.clear();
	}
	InstancedSprite& InstancedSpriteRenderer::Get(size_t index) const {
		return *m_instances.at(index);
	}

	const Texture* InstancedSpriteRenderer::GetTexture() const { return m_texture; }
	const Vector2f& InstancedSpriteRenderer::GetSize() const { return m_size; }
	const IntRect& InstancedSpriteRenderer::GetTextureRect() const { return m_texture_rect; }

	void InstancedSprite::SetTransform(const Matrix3x3& transform) {
		m_renderer->m_transforms.at(m_index) = transform;
	}
	const Matrix3x3& InstancedSprite::GetTransform() const {
		return m_renderer->m_transforms.at(m_index);
	}

	void InstancedSprite::SetColor(const Color& color) {
		m_renderer->m_colors.at(m_index) = color.GetNormalized();
	}
	Color InstancedSprite::GetColor() const {
		return Color(m_renderer->m_colors.at(m_index));
	}

	InstancedSpriteRenderer& InstancedSprite::GetRenderer() const { return *m_renderer; }
	size_t InstancedSprite::GetIndex() const { return m_index; }
}