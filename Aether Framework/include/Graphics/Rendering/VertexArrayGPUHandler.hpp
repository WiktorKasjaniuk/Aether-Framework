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

#pragma once

#include "../../System/Vector2.hpp"
#include "../../System/Vector3.hpp"
#include "../../System/Vector4.hpp"
#include "../Color.hpp"

#include "../../System/LogError.hpp"
#include "../../Core/Preprocessor.hpp"

#include <vector>
#include <cstdint>

#define AE_VERTEX_ARRAY_IS_BOUND(handler) \
	(handler.m_vao_id == ae::internal::VertexArrayGPUHandler::s_bound_vao_id)

namespace ae {
	namespace internal {

		// Important informations for adding layout
		struct VertexAttributeTypeInfo {
			int gl_type;
			unsigned char component_count;

			template<typename T>
			static constexpr VertexAttributeTypeInfo Create(const T&) { return VertexAttributeTypeInfo(GetOpenGLType<T>(), 1); }

			template<typename T>
			static constexpr VertexAttributeTypeInfo Create(const Vector2<T>&) { return VertexAttributeTypeInfo(GetOpenGLType<T>(), 2); }

			template<typename T>
			static constexpr VertexAttributeTypeInfo Create(const Vector3<T>&) { return VertexAttributeTypeInfo(GetOpenGLType<T>(), 3); }

			template<typename T>
			static constexpr VertexAttributeTypeInfo Create(const Vector4<T>&) { return VertexAttributeTypeInfo(GetOpenGLType<T>(), 4); }

			template<>
			static constexpr VertexAttributeTypeInfo Create<Color>(const Color&) { return VertexAttributeTypeInfo(GetOpenGLType<unsigned char>(), 4); }

		private:
			constexpr VertexAttributeTypeInfo(std::int32_t gl_type, unsigned char component_count)
				: gl_type(gl_type), component_count(component_count) {}

			// Error
			template <typename T>
			static std::int32_t GetOpenGLType() {
				AE_ASSERT_FALSE("Passed type or vector of type '" << typeid(T).name() << "' is not acceptable as vertex layout");
				LogError("Passed type or vector of type '" + std::string(typeid(T).name()) + "' is not acceptable as vertex layout", true);
				return 0;
			}

			template <>
			static constexpr std::int32_t GetOpenGLType<char>() { return 0x1400; } // GL_BYTE
			
			template <>
			static constexpr std::int32_t GetOpenGLType<unsigned char>() { return 0x1401; } // GL_UNSIGNED_BYTE 
			
			template <>
			static constexpr std::int32_t GetOpenGLType<short>() { return 0x1402; } // GL_SHORT
			
			template <>
			static constexpr std::int32_t GetOpenGLType<unsigned short>() { return 0x1403; } // GL_UNSIGNED_SHORT
			
			template <>
			static constexpr std::int32_t GetOpenGLType<int>() { return 0x1404; } // GL_INT
			
			template <>
			static constexpr std::int32_t GetOpenGLType<unsigned int>() { return 0x1405; } // GL_UNSIGNED_INT

			template <>
			static constexpr std::int32_t GetOpenGLType<float>() { return 0x1406; } // GL_FLOAT

			template <>
			static constexpr std::int32_t GetOpenGLType<double>() { return 0x140A; } // GL_DOUBLE
		};

		struct VertexArrayGPUHandler {

			// Cache vertex attributes
			struct VertexAttribute {
				std::uint32_t location;
				VertexAttributeTypeInfo ati;
				bool normalize;
				size_t byte_size;
				size_t byte_offset;

				VertexAttribute(std::uint32_t location, const VertexAttributeTypeInfo& ati, bool normalize, size_t byte_size, size_t byte_offset)
					: location(location), ati(ati), normalize(normalize), byte_size(byte_size), byte_offset(byte_offset) {}
			};
			
			std::vector<VertexAttribute> m_attributes;

			// OpenGL object ids
			std::uint32_t m_vao_id, m_vbo_id, m_ebo_id;

			AE_DEBUG_ONLY(static std::uint32_t s_bound_vao_id);

			// Functions to communicate with OpenGL
			VertexArrayGPUHandler();
			VertexArrayGPUHandler(const VertexArrayGPUHandler& copy);
			VertexArrayGPUHandler(VertexArrayGPUHandler&& move) noexcept;
			~VertexArrayGPUHandler();

			void operator=(const VertexArrayGPUHandler& copy);
			void operator=(VertexArrayGPUHandler&& move) noexcept;

			void AddAttribute(std::uint32_t location, const VertexAttributeTypeInfo& ati, bool normalize, size_t byte_size, size_t byte_offset);

			void Bind() const;
			void Unbind() const;

			void Draw(std::int32_t draw_mode, size_t start_index_index, size_t indices_count) const;
			void DrawInstanced(std::int32_t draw_mode, size_t start_index_index, size_t indices_count, size_t instance_count) const;

			void UpdateReallocate(size_t vertices_byte_size, const void* vertices_data, size_t indices_byte_size, const void* indices_data) const;
			void Update(size_t vertices_byte_size, const void* vertices_data, size_t indices_byte_size, const void* indices_data) const;

		private:
			void AllocateBuffers();
			void DeallocateBuffers();

			void CopyFrom(const VertexArrayGPUHandler& copy);
			void MoveFrom(VertexArrayGPUHandler&& move) noexcept;
		};

	} // internal
}