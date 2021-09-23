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


#include "Graphics/Rendering/Shape.hpp"
#include "Structure/AssetManager.hpp"
#include "Core/Preprocessor.hpp"

namespace ae {

	Shape::Shape(const std::vector<VertexPos>& vertices, const std::vector<unsigned int>& indices, const Color& color, DrawMode draw_mode) {
		m_color = color;
		SetDrawMode(draw_mode);
		
		m_vertices.Append(vertices, indices);

		m_vertices.Bind();
		m_vertices.AddLayout<Vector2f>(0, offsetof(VertexPos, position), false);
	}
	Shape::Shape(const Color& color, DrawMode draw_mode) {
		m_color = color;
		SetDrawMode(draw_mode);

		m_vertices.Bind();
		m_vertices.AddLayout<Vector2f>(0, offsetof(VertexPos, position), false);
	}

	void Shape::Draw(const Matrix3x3& transform) const {
		Draw(DefaultAssets->color_shader, transform);
	}
	void Shape::Draw(const Shader& shader, const Matrix3x3& transform) const {
		shader.Bind();
		shader.SetUniform.Mat3x3("u_mvp", transform.GetArray());
		shader.SetUniform.Vec4f("u_color", m_color.GetNormalized());

		m_vertices.Bind();
		m_vertices.Draw();
	}
	void Shape::Draw(const std::function<void(const Color&, const VertexArray<VertexPos>&)>& draw) const {
		draw(m_color, m_vertices);
	}

	void Shape::SetColor(const Color& color) { m_color = color; }
	const Color& Shape::GetColor() const { return m_color; }

	void Shape::SetPositions(const std::vector<Vector2f>& positions) {
		AE_ASSERT(m_vertices.GetVertices().size() == positions.size(), "Could not set shape positions, vector length is invalid");
		m_vertices.SetVertices(reinterpret_cast<const std::vector<VertexPos>&>(positions));
	}
	std::vector<Vector2f> Shape::GetPositions() const {
		return reinterpret_cast<const std::vector<Vector2f>&>(m_vertices.GetVertices());
	}

	void Shape::SetVertices(const std::vector<VertexPos>& vertices) {
		m_vertices.SetVertices(vertices);
	}
	void Shape::SetIndices(const std::vector<unsigned int>& indices) {
		m_vertices.SetIndices(indices);
	}

	void Shape::SetDrawMode(DrawMode mode) {
		m_vertices.SetDrawMode(mode);
	}
	DrawMode Shape::GetDrawMode() const {
		return m_vertices.GetDrawMode();
	}

	const std::vector<VertexPos>& Shape::GetVertices() const { return m_vertices.GetVertices(); }
	const std::vector<unsigned int>& Shape::GetIndices() const { return m_vertices.GetIndices(); }

}