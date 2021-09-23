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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// BatchSpriteRenderer
///
/// General Idea:
///		BSR makes rendering faster by batching sprites and rendering they with as little render calls as the machine allows.
///		It stores and manages all instances and their parameters.
///
/// Creation and Destruction:
///		Batch sprites can be created only at the back of the renderer, meaning newly created sprite will be drawn last (at top).
///		Destruction can be done by sprite's index or pointer.'
/// 
/// Parameters:
///		Each BatchSprite has it's own transform, size, texture rect and color for each vertex,
///		although all share the same texture.
/// 
///		All parameters must be set manually, as setting the renderer's texture does not update batches' sizes and texture rects.
///
/// Sorting:
///		Sort() function is optimized for every-frame usage,
///		all bathces are sorted using insertion sort, 
///		their parameters can be easily reached and / or modified via passed compare function.
///		Sprites' indices are modified after sorting.
///
/// Drawing:
///		Drawing is done by passing global transform for all batches, custom shader can be set.
///		Also, user-defined draw function can be used instead.
/// 
///		If texture was not set, 1 px white texture will be used instead.
/// 
/// Used Shaders:
///		Custom Parameters:
///			depend on machine:
///				constexpr size_t mat3_comp_count = 12, uint_comp_count = 1;
///				batch_sprite_max_draws_per_call = static_cast<size_t>((Shader::RetrieveInfo.MaxUniformComponentsVertex() - mat3_comp_count - uint_comp_count) / mat3_comp_count);
/// 
///			u_rendered_batches - states how many batches were drawn after latest flush.
/// 
///		Vertex Shader :
///			#version 460 core 
///			 
///			layout(location = 0) in vec2 a_position; 
///			layout(location = 1) in vec2 a_texcoords; 
///			layout(location = 2) in vec4 a_color; 
///			 
///			out vec2 v_texcoords; 
///			out vec4 v_color; 
///			 
///			uniform uint u_rendered_batches; 
///			uniform mat3 u_models[batch_sprite_max_draws_per_call]; // custom parameter set on shader creation, it depends on machine
///			uniform mat3 u_vp; 
///			 
///			void main() 
///			{ 
///				gl_Position = vec4(u_vp * u_models[gl_VertexID/4 - u_rendered_batches] * vec3(a_position, 1.0), 1.0); 
///				v_texcoords = a_texcoords; 
///				v_color = a_color; 
///			}
///		
///		Fragment Shader:
///			#version 460 core 
///			 
///			layout(location = 0) out vec4 a_color; 
///			 
///			in vec2 v_texcoords; 
///			in vec4 v_color; 
///			 
///			uniform sampler2D u_texture; 
///			 
///			void main() 
///			{ 
///				vec2 normalized_coords = v_texcoords / vec2(textureSize(u_texture, 0)); 
///				a_color = texture(u_texture, normalized_coords) * v_color; 
///			}
///		
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VertexArray.hpp"
#include "../Matrix3x3.hpp"
#include "../Color.hpp"
#include "../Texture.hpp"
#include "../Shader.hpp"
#include "../../Structure/Camera.hpp"

#include <vector>
#include <memory>
#include <functional>
#include <algorithm>

namespace ae {
	class BatchSprite;

	// Vertex Type
	struct VertexBatchSprite {
		Vector2f position;
		Vector2f texcoords;
		Vector4f color;

		VertexBatchSprite() = default;

		VertexBatchSprite(unsigned int quad_render_order, const Vector2f& position, const Vector2f& texcoords, const Vector4f& color)
			: position(position), texcoords(texcoords), color(color) {}
	};

	// Renderer
	class BatchSpriteRenderer {
		friend class BatchSprite;

	public:
		BatchSpriteRenderer();
		BatchSpriteRenderer(const BatchSpriteRenderer& copy);
		BatchSpriteRenderer(BatchSpriteRenderer&& move) noexcept;

		BatchSpriteRenderer& operator=(const BatchSpriteRenderer& copy);
		BatchSpriteRenderer& operator=(BatchSpriteRenderer&& move) noexcept;

		BatchSprite& CreateBack(const Matrix3x3& transform = Matrix3x3::Identity);

		void Destroy(size_t index);
		void Destroy(BatchSprite& batch);
		void Clear();

		void Draw(const Shader& shader, const Matrix3x3& transform = Camera.GetProjViewMatrix()) const;
		void Draw(const Matrix3x3& transform = Camera.GetProjViewMatrix()) const;
		void Draw(const std::function<void(const Texture*, size_t batch_count, const std::vector<Matrix3x3>&, const VertexArray<VertexBatchSprite>&)>& draw) const;

		template <typename ...HandlerTypes>
		void Draw(const std::function<void(const Texture*, size_t batch_count, const std::vector<Matrix3x3>&, const VertexArray<VertexBatchSprite>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const;

		void SetTexture(const Texture& texture);
		const Texture* GetTexture() const;

		BatchSprite& Get(size_t index) const;
		size_t GetCount() const;

		void Sort(const std::function<bool(const BatchSprite& left, const BatchSprite& right)>& compare);

	private:
		const Texture* m_texture = nullptr;

		VertexArray<VertexBatchSprite> m_vertices;
		std::vector<std::unique_ptr<BatchSprite>> m_batches;
		std::vector<Matrix3x3> m_transforms;
	};

	// Batch
	class BatchSprite {
		friend class BatchSpriteRenderer;

	public:
		~BatchSprite() = default;

		void SetTransform(const Matrix3x3 & transform);
		const Matrix3x3& GetTransform() const;

		void SetSize(const Vector2f& size);
		const Vector2f& GetSize() const;

		void SetColors(const Color& v0, const Color& v1, const Color& v2, const Color& v3);
		std::tuple<Color, Color, Color, Color> GetColors() const;

		void SetTextureRect(const IntRect& rect);
		IntRect GetTextureRect() const;

		BatchSpriteRenderer& GetRenderer() const;
		size_t GetIndex() const;

	private:
		BatchSpriteRenderer* m_renderer;
		size_t m_index;

		BatchSprite() = default;
		BatchSprite(const BatchSprite&) = delete;
		BatchSprite(BatchSprite&&) = delete;
	};

	// Previous template paramters
	template <typename ...HandlerTypes>
	void BatchSpriteRenderer::Draw(const std::function<void(const Texture*, size_t batch_count, const std::vector<Matrix3x3>&, const VertexArray<VertexBatchSprite>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const {
		draw(m_texture, GetCount(), m_transforms, m_vertices, handlers...);
	}
}