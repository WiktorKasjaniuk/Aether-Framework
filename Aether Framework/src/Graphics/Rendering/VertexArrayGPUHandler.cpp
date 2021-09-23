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

#include "Graphics/Rendering/VertexArrayGPUHandler.hpp"
#include "Core/OpenGLCalls.hpp"

#include <glad/glad.h>

namespace ae {
	namespace internal {

		AE_DEBUG_ONLY(std::uint32_t VertexArrayGPUHandler::s_bound_vao_id = 0);

		VertexArrayGPUHandler::VertexArrayGPUHandler() {
			AllocateBuffers();
		}

		VertexArrayGPUHandler::VertexArrayGPUHandler(const VertexArrayGPUHandler& copy) {
			AllocateBuffers();
			CopyFrom(copy);
		}

		VertexArrayGPUHandler::VertexArrayGPUHandler(VertexArrayGPUHandler&& move) noexcept {
			MoveFrom(std::move(move));
		}
		VertexArrayGPUHandler::~VertexArrayGPUHandler() {
			DeallocateBuffers();
		}

		void VertexArrayGPUHandler::operator=(const VertexArrayGPUHandler& copy) {
			CopyFrom(copy);
		}

		void VertexArrayGPUHandler::operator=(VertexArrayGPUHandler&& move) noexcept {
			DeallocateBuffers();
			MoveFrom(std::move(move));
		}

		void VertexArrayGPUHandler::CopyFrom(const VertexArrayGPUHandler& copy){
			GLint size;

			// Copy VBO
			AE_GL_LOG(glBindBuffer(GL_COPY_READ_BUFFER, copy.m_vbo_id));
			AE_GL_LOG(glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size));

			AE_GL_LOG(glBindBuffer(GL_COPY_WRITE_BUFFER, m_vbo_id));
			AE_GL_LOG(glBufferData(GL_COPY_WRITE_BUFFER, size, NULL, GL_DYNAMIC_DRAW));

			AE_GL_LOG(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size));

			// Copy EBO
			AE_GL_LOG(glBindBuffer(GL_COPY_READ_BUFFER, copy.m_ebo_id));
			AE_GL_LOG(glGetBufferParameteriv(GL_COPY_READ_BUFFER, GL_BUFFER_SIZE, &size));

			AE_GL_LOG(glBindBuffer(GL_COPY_WRITE_BUFFER, m_ebo_id));
			AE_GL_LOG(glBufferData(GL_COPY_WRITE_BUFFER, size, NULL, GL_DYNAMIC_DRAW));

			AE_GL_LOG(glCopyBufferSubData(GL_COPY_READ_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, size));

			// Bind buffers to VAO
			Bind();

			// Copy attributes
			if (!m_attributes.empty()) {

				for (const VertexAttribute& attribute : m_attributes) {
					AE_GL_LOG(glDisableVertexAttribArray(attribute.location));
				}

				m_attributes.clear();
			}
				
			for (const VertexAttribute& attribute : copy.m_attributes)
				AddAttribute(attribute.location, attribute.ati, attribute.normalize, attribute.byte_size, attribute.byte_offset);
		}
		void VertexArrayGPUHandler::MoveFrom(VertexArrayGPUHandler&& move) noexcept {

			m_attributes = std::move(move.m_attributes);

			m_vao_id = move.m_vao_id;
			m_vbo_id = move.m_vbo_id;
			m_ebo_id = move.m_ebo_id;

			move.m_vao_id = 0;
			move.m_vbo_id = 0;
			move.m_ebo_id = 0;
		}

		void VertexArrayGPUHandler::AllocateBuffers() {
			AE_GL_LOG(glGenVertexArrays(1, &m_vao_id));
			AE_GL_LOG(glGenBuffers(1, &m_vbo_id));
			AE_GL_LOG(glGenBuffers(1, &m_ebo_id));
		}
		void VertexArrayGPUHandler::DeallocateBuffers() {
			GLuint buffers[2] = { m_vbo_id, m_ebo_id };
			AE_GL_LOG(glDeleteBuffers(2, buffers));
			AE_GL_LOG(glDeleteVertexArrays(1, &m_vao_id));
		}

		void VertexArrayGPUHandler::AddAttribute(std::uint32_t location, const VertexAttributeTypeInfo& ati, bool normalize, size_t byte_size, size_t byte_offset) {
			AE_ASSERT_WARNING(
				(
					std::find_if(m_attributes.begin(), m_attributes.end(),
					[location](VertexAttribute& attribute) {
						return (attribute.location == location);
					})
					== m_attributes.end()
				),
				"Vertex Array layout location '" << location << "' is already added"
			);
			
			m_attributes.emplace_back(location, ati, normalize, byte_size, byte_offset);

			AE_GL_LOG(glVertexAttribPointer(location, ati.component_count, ati.gl_type, normalize, byte_size, reinterpret_cast<void*>(byte_offset)));
			AE_GL_LOG(glEnableVertexAttribArray(location));
		}

		void VertexArrayGPUHandler::Bind() const {
			AE_DEBUG_ONLY(s_bound_vao_id = m_vao_id);

			AE_GL_LOG(glBindVertexArray(m_vao_id));
			AE_GL_LOG(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id));
			AE_GL_LOG(glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id));
		}
		void VertexArrayGPUHandler::Unbind() const {
			AE_DEBUG_ONLY(s_bound_vao_id = 0);

			AE_GL_LOG(glBindVertexArray(0));
			AE_GL_LOG(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			AE_GL_LOG(glBindBuffer(GL_ARRAY_BUFFER, 0));
		}
		void VertexArrayGPUHandler::Draw(std::int32_t draw_mode, size_t start_index_index, size_t indices_count) const {
			AE_GL_LOG(glDrawElements(draw_mode, indices_count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(start_index_index * sizeof(std::uint32_t))));
		}
		void VertexArrayGPUHandler::DrawInstanced(std::int32_t draw_mode, size_t start_index_index, size_t indices_count, size_t instance_count) const {
			AE_GL_LOG(glDrawElementsInstanced(draw_mode, indices_count, GL_UNSIGNED_INT, reinterpret_cast<const void*>(start_index_index * sizeof(std::uint32_t)), instance_count));
		}

		void VertexArrayGPUHandler::UpdateReallocate(size_t vertices_byte_size, const void* vertices_data, size_t indices_byte_size, const void* indices_data) const {

			// Ensure empty buffers = NULL
			const void* new_vertex_data = (vertices_byte_size != 0) ? vertices_data : NULL;
			const void* new_indices_data = (indices_byte_size != 0) ? indices_data  : NULL;

			// Vertices
			AE_GL_LOG(glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id));
			AE_GL_LOG(glBufferData(GL_ARRAY_BUFFER, vertices_byte_size, new_vertex_data, GL_DYNAMIC_DRAW));

			// Indices
			AE_GL_LOG(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id));
			AE_GL_LOG(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_byte_size, new_indices_data, GL_DYNAMIC_DRAW));

		}
		void VertexArrayGPUHandler::Update(size_t vertices_byte_size, const void* vertices_data, size_t indices_byte_size, const void* indices_data) const {

			// Vertices
			AE_GL_LOG(glBindBuffer(GL_ARRAY_BUFFER, m_vbo_id));
			AE_GL_LOG(glBufferSubData(GL_ARRAY_BUFFER, 0, vertices_byte_size, vertices_data));

			// Indices
			AE_GL_LOG(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo_id));
			AE_GL_LOG(glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices_byte_size, indices_data));
		}
	}
}