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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Instanced Sprite Renderer
///
/// General Idea:
///		ISR makes rendering faster by using instanced rendering.
///		It stores and manages all instances and their parameters.
/// 
/// Parameters:
///		Renderer stores a quad as vertices (one size, one texturecoords) and one texture for all the instances
///		and a color, and transform for each individual instance.
///		
///		First time when the texture is set,
///			if size was not set before it becomes texture size,
///			and if texture rectangle was not set it becomes IntRect({0, 0}, texture size).
/// 
/// Sorting:
///		Sort() function is optimized for every-frame usage,
///		all instances are sorted using insertion sort, 
///		their parameters can be easily reached and / or modified via passed compare function.
///		Instances' indices are modified after sorting.
/// 
/// Drawing:
///		Drawing is done by passing global transform for all instances, custom shader can be set.
///		Also, user-defined draw function can be used instead.
/// 
///		If texture was not set, 1 px white texture will be used instead.
/// 
/// Used Shaders:
/// 
///		Custom Parameters (depend on machine):
///			constexpr size_t mat3_comp_count = 12, vec4_comp_count = 4;
///			instanced_sprite_max_draws_per_call = static_cast<size_t>((Shader::RetrieveInfo.MaxUniformComponentsVertex() - mat3_comp_count) / (mat3_comp_count + vec4_comp_count));
///
///		Vertex Shader:
///			#version 460 core 
///			 
///			layout(location = 0) in vec2 a_position; 
///			layout(location = 1) in vec2 a_texcoords; 
///			 
///			out vec2 v_texcoords; 
///			out vec4 v_color; 
///			 
///			uniform vec4 u_colors[instanced_sprite_max_draws_per_call]; // custom parameter set on shader creation, it depends on machine
///			uniform mat3 u_models[instanced_sprite_max_draws_per_call]; 
///			uniform mat3 u_vp; 
///			 
///			void main() 
///			{ 
///				gl_Position = vec4(u_vp * u_models[gl_InstanceID] * vec3(a_position, 1.0), 1.0); 
///				v_texcoords = a_texcoords; 
///				v_color = u_colors[gl_InstanceID]; 
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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
	class InstancedSprite;

	// Renderer
	class InstancedSpriteRenderer {
		friend class InstancedSprite;

	public:
		InstancedSpriteRenderer();
		InstancedSpriteRenderer(const InstancedSpriteRenderer& copy);
		InstancedSpriteRenderer(InstancedSpriteRenderer&& move) noexcept;

		InstancedSpriteRenderer& operator=(const InstancedSpriteRenderer& copy);
		InstancedSpriteRenderer& operator=(InstancedSpriteRenderer&& move) noexcept;

		InstancedSprite& Create(size_t index, const Matrix3x3& transform = Matrix3x3::Identity, const Color& color = Color());
		InstancedSprite& CreateBack(const Matrix3x3& transform = Matrix3x3::Identity, const Color& color = Color());
		void Destroy(size_t index);
		void Destroy(InstancedSprite* instance);
		void Clear();

		InstancedSprite& Get(size_t index) const;
		size_t GetCount() const;

		void Sort(const std::function<bool(const InstancedSprite& left, const InstancedSprite& right)>& compare);

		void Draw(const Shader& shader, const Matrix3x3& transform = Camera.GetProjViewMatrix()) const;
		void Draw(const Matrix3x3& transform = Camera.GetProjViewMatrix()) const;
		void Draw(const std::function<void(const Texture*, size_t instance_count, const std::vector<Matrix3x3>&, const std::vector<Vector4f>&, const VertexArray<VertexPosTex>&)>& draw) const;

		template <typename ...HandlerTypes>
		void Draw(const std::function<void(const Texture*, size_t instance_count, const std::vector<Matrix3x3>&, const std::vector<Vector4f>&, const VertexArray<VertexPosTex>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const;
		
		void SetTexture(const Texture& texture);
		void SetSize(const Vector2f& size);
		void SetTextureRect(const IntRect& rect);

		const Texture* GetTexture() const;
		const Vector2f& GetSize() const;
		const IntRect& GetTextureRect() const;

	private:
		VertexArray<VertexPosTex> m_vertices;
		Vector2f m_size;
		const Texture* m_texture = nullptr;
		IntRect m_texture_rect;

		void UpdateTextureCoords();

		std::vector<std::unique_ptr<InstancedSprite>> m_instances;
		std::vector<Matrix3x3> m_transforms;
		std::vector<Vector4f> m_colors;
	};

	// Instance
	class InstancedSprite {
		friend class InstancedSpriteRenderer;

	public:
		~InstancedSprite() = default;

		void SetTransform(const Matrix3x3& transform);
		const Matrix3x3& GetTransform() const;

		void SetColor(const Color& color);
		Color GetColor() const;

		InstancedSpriteRenderer& GetRenderer() const;
		size_t GetIndex() const;

	private:
		InstancedSpriteRenderer* m_renderer;
		size_t m_index;

		InstancedSprite() = default;
		InstancedSprite(const InstancedSprite&) = delete;
		InstancedSprite(InstancedSprite&&) = delete;
	};

	// Previous template definitions
	template <typename ...HandlerTypes>
	void InstancedSpriteRenderer::Draw(const std::function<void(const Texture*, size_t instance_count, const std::vector<Matrix3x3>&, const std::vector<Vector4f>&, const VertexArray<VertexPosTex>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const {
		draw(m_texture, GetCount(), m_transforms, m_colors, m_vertices, handlers...);
	}

}