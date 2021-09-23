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

/////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Shape
///
/// General Idea:
///		Shape is just a wrapper class around VertexArray, 
///		used to quickly get one or more simple drawables on the screen.
/// 
/// Parameters:
///		Shape's color, which is defaultly white
///		and drawing mode, which is defaulty DrawMode::Triangles (GL_TRIANGLES).
/// 
///		Shape's vertices contain only the position
///		and indices that state in which order the vertices should be drawn.
///		If mode is set to DrawMode::Triangles, vertex draw order should be set for all the triangles,
///		example: 
///			drawing a square of vertices (v0, v1, v2, v3), 
///			with mode set to DrawMode::Triangles, we need to draw 2 triangles to get one square,
///			so the order(indices) should be set to: (0, 1, 2, 2, 3, 0)
///		
/// Drawing:
///		Drawing is done by passing a transform matrix and / or custom shader,
///		or user-defined draw function can be passed instead.
/// 
/// Used Shaders:
///		Vertex Shader:
///			#version 460 core 
///					
///			layout(location = 0) in vec2 a_position; 
///					
///			uniform mat3 u_mvp; 
///					
///			void main() 
///			{ 
///				gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0); 
///			}
/// 
///		Fragment Shader:
///			#version 460 core 
///					 
///			layout(location = 0) out vec4 a_color; 
///					 
///			in vec4 v_color; 
///					 
///			uniform vec4 u_color; 
///					 
///			void main() 
///			{  
///				a_color = u_color; 
///			}
/// 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "VertexArray.hpp"

#include "../Color.hpp"
#include "../Matrix3x3.hpp"
#include "../Shader.hpp"
#include "../../Structure/Camera.hpp"

#include <functional>

namespace ae {
	
	class Shape {
	public:
		Shape(const std::vector<VertexPos>& vertices, const std::vector<unsigned int>& indices, const Color& color = Color::White, DrawMode draw_mode = DrawMode::Triangles);
		Shape(const Color& color = Color::White, DrawMode draw_mode = DrawMode::Triangles);
		Shape(const Shape&) = default;
		Shape(Shape&&) = default;
		~Shape() = default;

		Shape& operator=(const Shape&) = default;
		Shape& operator=(Shape&&) = default;

		void Draw(const Matrix3x3& transform = Camera.GetProjViewMatrix()) const;
		void Draw(const Shader& shader, const Matrix3x3& transform = Camera.GetProjViewMatrix()) const;
		void Draw(const std::function<void(const Color&, const VertexArray<VertexPos>&)>& draw) const;

		template <typename ...HandlerTypes>
		void Draw(const std::function<void(const Color&, const VertexArray<VertexPos>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const;

		void SetColor(const Color& color);
		const Color& GetColor() const;

		void SetDrawMode(DrawMode mode);
		DrawMode GetDrawMode() const;

		void SetPositions(const std::vector<Vector2f>& positions);
		std::vector<Vector2f> GetPositions() const;

		void SetVertices(const std::vector<VertexPos>& vertices);
		void SetIndices(const std::vector<unsigned int>& indices);

		const std::vector<VertexPos>& GetVertices() const;
		const std::vector<unsigned int>& GetIndices() const;

	private:
		VertexArray<VertexPos> m_vertices;
		Color m_color;
	};

	// Previous template definitions
	template <typename ...HandlerTypes>
	void Shape::Draw(const std::function<void(const Color&, const VertexArray<VertexPos>&, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const {
		draw(m_color, m_vertices, handlers...);
	}
}