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


#include "Graphics/Rendering/BatchSpriteRenderer.hpp"
#include "Structure/AssetManager.hpp"

#include "System/Clock.hpp"

// allows sorting vertices as quads
struct QuadVertices {
	ae::VertexBatchSprite vertices[4];
};

namespace ae {

	BatchSpriteRenderer::BatchSpriteRenderer() {
		m_vertices.Bind();

		m_vertices.AddLayout<Vector2f>(0, offsetof(VertexBatchSprite, position), false);
		m_vertices.AddLayout<Vector2f>(1, offsetof(VertexBatchSprite, texcoords), false);
		m_vertices.AddLayout<Vector4f>(2, offsetof(VertexBatchSprite, color), false);
	}
	BatchSpriteRenderer::BatchSpriteRenderer(const BatchSpriteRenderer& copy) {
		*this = copy;
	}
	BatchSpriteRenderer::BatchSpriteRenderer(BatchSpriteRenderer&& move) noexcept {
		*this = std::move(move);
	}

	BatchSpriteRenderer& BatchSpriteRenderer::operator=(const BatchSpriteRenderer& copy) {
		m_texture = copy.m_texture;
		m_vertices = copy.m_vertices;
		m_transforms = copy.m_transforms;

		// copy batches
		m_batches.reserve(copy.m_batches.size());

		for (auto& batch_uptr : copy.m_batches) {
			BatchSprite* batch = new BatchSprite();
			batch->m_renderer = this;
			batch->m_index = batch_uptr.get()->m_index;

			m_batches.emplace_back(batch);
		}

		return *this;
	}
	BatchSpriteRenderer& BatchSpriteRenderer::operator=(BatchSpriteRenderer&& move) noexcept {
		m_texture = std::move(move.m_texture);
		m_vertices = std::move(move.m_vertices);
		m_transforms = std::move(move.m_transforms);
		m_batches = std::move(move.m_batches);

		for (auto& batch_uptr : m_batches)
			batch_uptr->m_renderer = this;

		return *this;
	}

	
	void BatchSpriteRenderer::Draw(const Matrix3x3& transform) const {
		Draw(ae::DefaultAssets->batch_sprite_shader, transform);
	}
	void BatchSpriteRenderer::Draw(const Shader& shader, const Matrix3x3& transform) const {
		shader.Bind();
		shader.SetUniform.Mat3x3("u_vp", transform.GetArray());

		if (m_texture)
			m_texture->Bind(0);
		else
			DefaultAssets->white_pixel_texture.Bind(0);
		
		shader.SetUniform.Sampler2D("u_texture", 0);

		m_vertices.Bind();
		size_t max_draws_per_call = DefaultAssets->batch_sprite_max_draws_per_call;

		for (size_t drawn_batches(0); drawn_batches < GetCount(); drawn_batches += max_draws_per_call) {
			size_t next_draw_count = std::min(max_draws_per_call, GetCount() - drawn_batches);

			shader.SetUniform.UnsignedInt("u_rendered_batches", drawn_batches);

			const float* models_data = reinterpret_cast<const float*>(m_transforms.at(drawn_batches).GetArray());
			shader.SetUniform.Mat3x3("u_models", models_data, 0, next_draw_count);

			m_vertices.Draw(drawn_batches * 6, next_draw_count * 6);
		}

	}
	void BatchSpriteRenderer::Draw(const std::function<void(const Texture*, size_t batch_count, const std::vector<Matrix3x3>&, const VertexArray<VertexBatchSprite>&)>& draw) const {
		draw(m_texture, GetCount(), m_transforms, m_vertices);
	}
	BatchSprite& BatchSpriteRenderer::CreateBack(const Matrix3x3& transform) {
		BatchSprite* batch = m_batches.emplace_back(new BatchSprite()).get();

		batch->m_renderer = this;
		batch->m_index = GetCount() - 1;

		m_transforms.emplace_back(transform);
		unsigned int i = m_vertices.GetVertices().size();
		m_vertices.Append(
			{
				VertexBatchSprite(),
				VertexBatchSprite(),
				VertexBatchSprite(),
				VertexBatchSprite()
			},
			{
				i	 , i + 1, i + 2,
				i + 2, i + 3, i
			}
		);

		return *batch;
	}

	void BatchSpriteRenderer::Destroy(BatchSprite& batch) {
		Destroy(batch.m_index);
	}
	void BatchSpriteRenderer::Destroy(size_t index) {
		m_transforms.erase(m_transforms.begin() + index);
		m_batches.erase(m_batches.begin() + index);

		// update vertices
		m_vertices.EraseVertices(index * 4, index * 4 + 3);
		
		// update indices
		auto& indices = m_vertices.GetCPUIndices();
		m_vertices.EnsureSizeUpdate();

		indices.erase(indices.begin() + (index * 6), indices.begin() + (index + 1) * 6);

		for (size_t i(index * 6); i < indices.size(); i++)
			indices.at(i) -= 4;

		// update other batches' positions
		for (size_t i(index); i < GetCount(); i++)
			m_batches.at(i)->m_index--;
	}
	void BatchSpriteRenderer::Clear() {
		m_batches.clear();
		m_transforms.clear();
		m_vertices.Clear();
	}

	void BatchSpriteRenderer::Sort(const std::function<bool(const BatchSprite& left, const BatchSprite& right)>& compare) {

		// batches, transforms and vertices are sort
		// vertex indices stay as they are

		// reinterpret vertices array as quads array
		std::vector<QuadVertices>& quad_vertices = 
			reinterpret_cast<std::vector<QuadVertices>&>(
				m_vertices.GetCPUVertices()
			);

		m_vertices.EnsureSizeUpdate();

		// insertion sort batches
		for (int marker = 1; marker < m_batches.size(); marker++) {

			std::unique_ptr<BatchSprite> batch = std::move(m_batches[marker]);
			int sub_marker = marker - 1;

			while (sub_marker >= 0 && compare(*m_batches[sub_marker], *batch)) {
				m_batches[sub_marker + 1] = std::move(m_batches[sub_marker]);

				sub_marker--;
			}
			m_batches[sub_marker + 1] = std::move(batch);
		}

		// create new order and update batches' m_index
		std::vector<size_t> new_order;
		new_order.reserve(GetCount());
		for (size_t i(0); i < GetCount(); i++) {
			BatchSprite& batch = *m_batches.at(i);

			new_order.push_back(batch.m_index);
			batch.m_index = i;
		}
		
		// safely reorder stuff by new order
		for (size_t i = 0; i < GetCount() - 1; i++) {
			if (new_order[i] == i)
				continue;

			size_t o;
			for (o = i + 1; o < new_order.size(); ++o)
				if (new_order[o] == i)
					break;

			std::swap(m_transforms[i], m_transforms[new_order[i]]);
			std::swap(quad_vertices[i], quad_vertices[new_order[i]]);

			std::swap(new_order[i], new_order[o]);
		}
	}

	void BatchSpriteRenderer::SetTexture(const Texture& texture) {
		if (!texture.WasLoaded())
			return;

		m_texture = &texture;
	}

	BatchSprite& BatchSpriteRenderer::Get(size_t index) const { return *m_batches[index]; }
	size_t BatchSpriteRenderer::GetCount() const { return m_batches.size(); }
	const Texture* BatchSpriteRenderer::GetTexture() const { return m_texture; }

	void BatchSprite::SetTransform(const Matrix3x3& transform) {
		m_renderer->m_transforms.at(m_index) = transform;
	}
	const Matrix3x3& BatchSprite::GetTransform() const {
		return m_renderer->m_transforms.at(m_index);
	}

	void BatchSprite::SetSize(const Vector2f& size) {
		auto& vertex_array = m_renderer->m_vertices;
		const auto& vertices = vertex_array.GetVertices();
		size_t vertex_index = m_index * 4;

		VertexBatchSprite v1 = vertices.at(vertex_index + 1);
		v1.position.x = size.x;

		VertexBatchSprite v2 = vertices.at(vertex_index + 2);
		v2.position = size;

		VertexBatchSprite v3 = vertices.at(vertex_index + 3);
		v3.position.y = size.y;

		vertex_array.SetVertex(vertex_index + 1, v1);
		vertex_array.SetVertex(vertex_index + 2, v2);
		vertex_array.SetVertex(vertex_index + 3, v3);
	}
	const Vector2f& BatchSprite::GetSize() const {
		const auto& vertices = m_renderer->m_vertices.GetVertices();
		return Vector2f(vertices.at(m_index * 4 + 3).position);
	}

	void BatchSprite::SetColors(const Color& v0, const Color& v1, const Color& v2, const Color& v3) {
		size_t index = m_index * 4;
		
		auto& vertex_array = m_renderer->m_vertices;
		const auto& vertices = vertex_array.GetVertices();

		// top left
		VertexBatchSprite vertex = vertices.at(index);
		vertex.color = v0.GetNormalized();
		vertex_array.SetVertex(index, vertex);

		index++;

		// top right
		vertex = vertices.at(index);
		vertex.color = v1.GetNormalized();
		vertex_array.SetVertex(index, vertex);

		index++;

		// bottom right
		vertex = vertices.at(index);
		vertex.color = v2.GetNormalized();
		vertex_array.SetVertex(index, vertex);

		index++;

		// bottom left
		vertex = vertices.at(index);
		vertex.color = v3.GetNormalized();
		vertex_array.SetVertex(index, vertex);
	}
	std::tuple<Color, Color, Color, Color> BatchSprite::GetColors() const {
		size_t index = m_index * 4;
		const auto& vertices = m_renderer->m_vertices.GetVertices();
		return {
			Color(vertices.at(index).color) , Color(vertices.at(index + 1).color),
			Color(vertices.at(index + 2).color) , Color(vertices.at(index + 3).color)
		};
	}

	void BatchSprite::SetTextureRect(const IntRect& rect){
		size_t index = m_index * 4;

		auto& vertex_array = m_renderer->m_vertices;
		const auto& vertices = vertex_array.GetVertices();

		// top left
		VertexBatchSprite vertex = vertices.at(index);
		vertex.texcoords = Vector2f(rect.left, rect.top);
		vertex_array.SetVertex(index, vertex);

		index++;

		// top right
		vertex = vertices.at(index);
		vertex.texcoords = Vector2f(rect.left + rect.width, rect.top);
		vertex_array.SetVertex(index, vertex);

		index++;

		// bottom right
		vertex = vertices.at(index);
		vertex.texcoords = Vector2f(rect.left + rect.width, rect.top + rect.height);
		vertex_array.SetVertex(index, vertex);

		index++;

		// bottom left
		vertex = vertices.at(index);
		vertex.texcoords = Vector2f(rect.left, rect.top + rect.height);
		vertex_array.SetVertex(index, vertex);
	}
	IntRect BatchSprite::GetTextureRect() const {
		size_t index = m_index * 4;
		const auto& vertices = m_renderer->m_vertices.GetVertices();

		return IntRect(Vector2i(vertices.at(index).texcoords), Vector2i(vertices.at(index + 3).texcoords));
	}

	BatchSpriteRenderer& BatchSprite::GetRenderer() const { return *m_renderer; }
	size_t BatchSprite::GetIndex() const { return m_index; }
}