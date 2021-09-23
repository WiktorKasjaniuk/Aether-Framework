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

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Vertex Array
///
/// Idea:
///		VertexArray is a class containing:
///			vertices and indices on CPU side,
///			and VBO, EBO and VAO on GPU side.
/// 
///			It is created with VertexType template parameter, which could be any given type that stores vertex attributes.
/// 
/// Usage:
///		Bind() and Unbind() are to be used for GPU communication (adding layout, drawing).
/// 
/// Layout / Attributes:
///		VertexType, the template parameter of VertexArray should contain all needed attributes.
///		Before adding layout, it is needed to Bind() VertexArray.
///		AddLayout<LayoutType>(location, byte_offset_to_member, normalize) is used to enable an attribute for shader usage:
///			LayoutType 
///				is the input type which can change as it moves to the shader,
///				it has to be one of: 
///					char, uchar, short, ushort, int, uint, float, double
///					or Vector2<T>, Vector3<T>, Vector4<T> made of them,
///					additionally, color is also accepted as Vector4<unsigned char>,
/// 
///				although layout types other than T = float are possible, it is recommended to either use floats or normalize values to floats.
/// 
///			location
///				is the shader's layout location for inputting attributes,
/// 
///			byte_offset_to_member
///				is the byte stride, offset to VertexType's member,
///				it is recommended to use operator offsetof(VertexType, member) for this purpose,
///			
///			normalize
///				indicates if the value should be normalized (value is divided by type's maximum value) and changed to a floating type.
/// 
///	Managing Data:
///		To clear both indices and vertices use Clear().
/// 
///		To append VertexArray you can use
///			Append(vector<VertexType>, vector<unsigned int>) to add new vertices and indices at the back,
///			SetVertices(vector<VertexType>) and SetIndices(vector<unsigned int>) allow setting any indices,
///			after updating data, it is copied to GPU just before drawing.
/// 
///		To retrieve const reference to the data use GetVertices() and GetIndices() functions.
///		 
///		To retrieve non-const reference use GetCPUVertices() and GetCPUIndices(),
///		BUT remember! Both do not ensure GPU update when drawing comes,
///		thus one of these functions must be also called:
///			> EnsureSizeUpdate() if one of the containers' size has changed
///			> EnsureParameterUpdate() if the size was not changed, but the container was modified in other way.
/// 
///		There are also few functions that let you modify data and automaticaly ensure updates.
/// 
/// Drawing:
///		Before drawing VertexArray has to be bound.
/// 
///		Mode:
///			Draw mode can chosen from DrawMode enum, default value is DrawMode::Triangles,
///			SetDrawMode(mode) and GetDrawMode() functions can be used.
/// 
///		Draw() uses the vertices and indices to draw GL_TRIANGLES with glDrawElements(),
///		Draw(instance_count) uses the same assets and draws them instanced, which could save some time if there are many similar objects, GL_TRIANGLES and glDrawElementsInstanced() are used
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../../Core/Preprocessor.hpp"
#include "VertexArrayGPUHandler.hpp"

#include <vector>

namespace ae {
	enum class DrawMode : std::int32_t {
		Points = 0x0000,

		Lines = 0x0001,
		LineLoop = 0x0002,
		LineStrip = 0x0003,

		Triangles = 0x0004,
		TriangleStrip = 0x0005,
		TriangleFan = 0x0006,

		LinesAdjacency = 0x000A,
		LineStripAdjacency = 0x000B,
		TrianglesAdjacency = 0x000C,
		TriangleStripAdjacency = 0x000D,

		Patches = 0x000E
	};

	// Default vertex types
	struct VertexPos { // VertexArray<Vector2f> would be enough
		Vector2f position;
	
		VertexPos() = default;
		VertexPos(const Vector2f& position) : position(position) {}
		VertexPos(float x, float y) : position(Vector2f(x,y)) {}

		void operator=(const Vector2f& position) {
			this->position = position;
		}
	};
	struct VertexPosTex {
		Vector2f position;
		Vector2f texcoords;

		VertexPosTex() = default;
		VertexPosTex(const Vector2f& position, const Vector2f& texcoords) : position(position), texcoords(texcoords) {}
	};

	// VertexArray
	template <typename VertexType>
	class VertexArray {
	public:
		VertexArray() = default;
		VertexArray(const VertexArray<VertexType>& copy);
		VertexArray(VertexArray<VertexType>&& move) noexcept;
		VertexArray(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices);

		bool operator==(const VertexArray<VertexType>& other) const;
		VertexArray<VertexType>& operator=(const VertexArray<VertexType>& other);
		VertexArray<VertexType>& operator=(VertexArray<VertexType>&& other) noexcept;

		template <typename LayoutType>
		void AddLayout(unsigned char location, size_t byte_offset_to_member, bool normalize) {
			AE_ASSERT(AE_VERTEX_ARRAY_IS_BOUND(m_handler), "VertexArray must be bound before adding layout");
			AE_ASSERT(byte_offset_to_member + sizeof(LayoutType) <= sizeof(VertexType), "Could not add layout to VertexArray, either byte offset is too large or type is incorrect");

			m_handler.AddAttribute(location, internal::VertexAttributeTypeInfo::Create(LayoutType()), normalize, sizeof(VertexType), byte_offset_to_member);
		}

		void SetDrawMode(DrawMode mode);
		DrawMode GetDrawMode() const;

		void Append(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices);

		void SetVertices(const std::vector<VertexType>& vertices);
		void SetIndices(const std::vector<unsigned int>& indices);

		void InsertVertices(const std::vector<VertexType>& vertices, size_t index);
		void InsertIndices(const std::vector<unsigned int>& indices, size_t index);

		void EraseVertices(size_t first_index, size_t last_index);
		void EraseIndices(size_t first_index, size_t last_index);

		void CopyIndices(std::vector<unsigned int>& output, std::vector<unsigned int>::iterator output_position, size_t first_index, size_t last_index) const;
		void CopyVertices(std::vector<VertexType>& output, typename std::vector<VertexType>::iterator output_position, size_t first_index, size_t last_index) const;

		void SetVertex(size_t array_index, const VertexType& vertex);
		void SetIndex(size_t array_index, unsigned int index);

		const VertexType& GetVertex(size_t array_index) const;
		unsigned int GetIndex(size_t array_index) const;

		const std::vector<VertexType>& GetVertices() const;
		const std::vector<unsigned int>& GetIndices() const;

		std::vector<VertexType>& GetCPUVertices();
		std::vector<unsigned int>& GetCPUIndices();

		void EnsureParameterUpdate() const;
		void EnsureSizeUpdate() const;

		void Clear();

		void Draw() const;
		void Draw(size_t start_index_index, size_t indices_count) const;
		void DrawInstanced(size_t instance_count) const;
		void DrawInstanced(size_t start_index_index, size_t indices_count, size_t instance_count) const;

		void Bind() const;
		void Unbind() const;
		
	private:
		internal::VertexArrayGPUHandler m_handler;

		mutable bool m_size_changed = false, m_parameter_updated = false;

		std::vector<VertexType> m_vertices;
		std::vector<unsigned int> m_indices;

		DrawMode m_draw_mode = DrawMode::Triangles;

		void Update() const;
	};

	// Previous template definitions
	template <typename VertexType>
	VertexArray<VertexType>::VertexArray(const VertexArray<VertexType>& copy) {
		*this = copy;
	}
	template <typename VertexType>
	VertexArray<VertexType>::VertexArray(VertexArray<VertexType>&& move) noexcept {
		*this = std::move(move);
	}
	template <typename VertexType>
	VertexArray<VertexType>::VertexArray(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices) {
		Append(vertices, indices);
	}
	template <typename VertexType>
	bool VertexArray<VertexType>::operator==(const VertexArray<VertexType>& copy) const {
		return (m_vertices = copy.m_vertices && m_indices == copy.m_indices);
	}

	template <typename VertexType>
	VertexArray<VertexType>& VertexArray<VertexType>::operator=(const VertexArray<VertexType>& other) {
		m_handler = other.m_handler;

		m_size_changed = other.m_size_changed;
		m_parameter_updated = other.m_parameter_updated;

		m_vertices = other.m_vertices;
		m_indices = other.m_indices;

		m_draw_mode = other.m_draw_mode;

		return *this;
	}
	template <typename VertexType>
	VertexArray<VertexType>& VertexArray<VertexType>::operator=(VertexArray<VertexType>&& other) noexcept {
		m_handler = std::move(other.m_handler);

		m_size_changed = other.m_size_changed;
		m_parameter_updated = other.m_parameter_updated;

		m_vertices = std::move(other.m_vertices);
		m_indices = std::move(other.m_indices);

		m_draw_mode = other.m_draw_mode;

		return *this;
	}

	template <typename VertexType>
	void VertexArray<VertexType>::SetDrawMode(DrawMode mode) { m_draw_mode = mode; }
	template <typename VertexType>
	DrawMode VertexArray<VertexType>::GetDrawMode() const { return m_draw_mode; }

	template <typename VertexType>
	void VertexArray<VertexType>::Append(const std::vector<VertexType>& vertices, const std::vector<unsigned int>& indices) {

		// Append cache data
		m_vertices.insert(m_vertices.end(), vertices.begin(), vertices.end());
		m_indices.insert(m_indices.end(), indices.begin(), indices.end());

		// Update
		m_size_changed = true;
	}

	template <typename VertexType>
	void VertexArray<VertexType>::SetVertices(const std::vector<VertexType>& vertices) {
		m_vertices = vertices;
		m_size_changed = true;

	}
	template <typename VertexType>
	void VertexArray<VertexType>::SetIndices(const std::vector<unsigned int>& indices) {
		m_indices = indices;
		m_size_changed = true;
	}
	template <typename VertexType>
	void VertexArray<VertexType>::InsertVertices(const std::vector<VertexType>& vertices, size_t index) {
		m_vertices.insert(m_vertices.begin() + index, vertices.begin(), vertices.end());
		m_size_changed = true;
	}

	template <typename VertexType>
	void VertexArray<VertexType>::InsertIndices(const std::vector<unsigned int>& indices, size_t index){
		m_indices.insert(m_indices.begin() + index, indices.begin(), indices.end());
		m_size_changed = true;
	}

	template <typename VertexType>
	void VertexArray<VertexType>::EraseVertices(size_t first_index, size_t last_index) {
		m_vertices.erase(m_vertices.begin() + first_index, m_vertices.begin() + (last_index + 1));
		m_size_changed = true;
	}
	template <typename VertexType>
	void VertexArray<VertexType>::EraseIndices(size_t first_index, size_t last_index) {
		m_indices.erase(m_indices.begin() + first_index, m_indices.begin() + (last_index + 1 ));
		m_size_changed = true;
	}

	template <typename VertexType>
	void VertexArray<VertexType>::CopyVertices(std::vector<VertexType>& output, typename std::vector<VertexType>::iterator output_position, size_t first_index, size_t last_index) const {
		output.insert(output_position, m_vertices.begin() + first_index, m_vertices.begin() + (last_index + 1));
	}

	template <typename VertexType>
	void VertexArray<VertexType>::CopyIndices(std::vector<unsigned int>& output, std::vector<unsigned int>::iterator output_position, size_t first_index, size_t last_index) const {
		output.insert(output_position, m_indices.begin() + first_index, m_indices.begin() + (last_index + 1));
	}

	template <typename VertexType>
	void VertexArray<VertexType>::SetVertex(size_t array_index, const VertexType& vertex) {
		m_vertices.at(array_index) = vertex;
		m_parameter_updated = true;
	}
	template <typename VertexType>
	void VertexArray<VertexType>::SetIndex(size_t array_index, unsigned int index) {
		m_indices.at(array_index) = index;
		m_parameter_updated = true;
	}

	template <typename VertexType>
	const VertexType& VertexArray<VertexType>::GetVertex(size_t array_index) const {
		return m_vertices.at(array_index);
	}
	template <typename VertexType>
	unsigned int VertexArray<VertexType>::GetIndex(size_t array_index) const {
		return m_indices.at(array_index);
	}
	template <typename VertexType>
	void VertexArray<VertexType>::EnsureParameterUpdate() const { m_parameter_updated = true; }

	template <typename VertexType>
	void VertexArray<VertexType>::EnsureSizeUpdate() const { m_size_changed = true; }

	template <typename VertexType>
	std::vector<VertexType>& VertexArray<VertexType>::GetCPUVertices() { return m_vertices; }

	template <typename VertexType>
	std::vector<unsigned int>& VertexArray<VertexType>::GetCPUIndices() { return m_indices; }

	template <typename VertexType>
	const std::vector<VertexType>& VertexArray<VertexType>::GetVertices() const { return m_vertices; }

	template <typename VertexType>
	const std::vector<unsigned int>& VertexArray<VertexType>::GetIndices() const { return m_indices; }

	template <typename VertexType>
	void VertexArray<VertexType>::Clear() {
		if (!m_vertices.empty() || !m_indices.empty()) {
			m_vertices.clear();
			m_indices.clear();
			m_size_changed = true;
		}
	}

#define AE_VERTEX_ARRAY_INDICES_VALID \
	(std::find_if(m_indices.begin(), m_indices.end(), \
		[this](unsigned int index) { \
			return (index >= m_vertices.size()); \
		} \
	) == m_indices.end())

	template <typename VertexType>
	void VertexArray<VertexType>::Draw() const {
		AE_ASSERT(AE_VERTEX_ARRAY_INDICES_VALID, "Could not draw vertex array, found an invalid index (index >= vertices.size())");
		Draw(0, m_indices.size());
	}

	template <typename VertexType>
	void VertexArray<VertexType>::Draw(size_t start_index_index, size_t indices_count) const {
		AE_ASSERT(AE_VERTEX_ARRAY_IS_BOUND(m_handler), "Vertex Array must be bound before drawing");
		AE_ASSERT(AE_VERTEX_ARRAY_INDICES_VALID, "Could not draw vertex array, found an invalid index (index >= vertices.size())");
		Update();
		m_handler.Draw(static_cast<std::int32_t>(m_draw_mode), start_index_index, indices_count);
	}

	template <typename VertexType>
	void VertexArray<VertexType>::DrawInstanced(size_t instance_count) const {
		AE_ASSERT(AE_VERTEX_ARRAY_INDICES_VALID, "Could not draw vertex array instanced, found an invalid index (index >= vertices.size())");
		DrawInstanced(0, m_indices.size(), instance_count);
	}
	template <typename VertexType>
	void VertexArray<VertexType>::DrawInstanced(size_t start_index_index, size_t indices_count, size_t instance_count) const {
		AE_ASSERT(AE_VERTEX_ARRAY_IS_BOUND(m_handler), "Vertex Array must be bound before drawing");
		AE_ASSERT(AE_VERTEX_ARRAY_INDICES_VALID, "Could not draw vertex array instanced, found an invalid index (index >= vertices.size())");
		Update();
		m_handler.DrawInstanced(static_cast<std::int32_t>(m_draw_mode), start_index_index, indices_count, instance_count);
	}

	template <typename VertexType>
	void VertexArray<VertexType>::Bind() const {
		m_handler.Bind();
	}

	template <typename VertexType>
	void VertexArray<VertexType>::Unbind() const {
		m_handler.Unbind();
	}

	template <typename VertexType>
	void VertexArray<VertexType>::Update() const {
		if (m_size_changed) {
			m_handler.UpdateReallocate(
				sizeof(VertexType)   * m_vertices.size(), m_vertices.data(), 
				sizeof(unsigned int) * m_indices.size(),  m_indices.data()
			);

			m_size_changed = false;
			m_parameter_updated = false;
		}
		else if (m_parameter_updated) {
			m_handler.Update(
				sizeof(VertexType) * m_vertices.size(), m_vertices.data(),
				sizeof(unsigned int) * m_indices.size(), m_indices.data()
			);

			m_parameter_updated = false;
		}
	}

}