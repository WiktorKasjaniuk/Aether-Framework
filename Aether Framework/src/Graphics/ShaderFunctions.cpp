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


#include "Graphics/Shader.hpp"
#include "Graphics/ShaderFunctions.hpp"
#include "Core/OpenGLCalls.hpp"
#include "System/LogError.hpp"

#include <glad/glad.h>

#define AE_SHADER_UNIFORM_TEXTURE_BIND_ASSERT(_slot) \
	AE_ASSERT_WARNING(_slot < m_shader->RetrieveInfo.MaxTextureSlotsFragment(), "Texture slot '" << _slot << "' is beyond fragment shader limit '" << m_shader->RetrieveInfo.MaxTextureSlotsFragment() << '\'');

#define AE_SHADER_UNIFORM_ASSERT_LOAD \
	AE_ASSERT(m_shader->m_shader_id != 0, "Shader was not loaded before setting an uniform")

#define AE_SHADER_UNIFORM_ASSERT_WARNING_BIND \
	AE_ASSERT_WARNING(m_shader->m_shader_id == m_shader->s_bound_shader_id, "Shader was not bound before setting an uniform")

#define AE_SHADER_UNIFORM_ASSERT_FIND \
	if (m_shader->m_uniform_locations.find(name) == m_shader->m_uniform_locations.end()) { \
		std::string error_info = '\''+ name + "' uniform location could not be found, possible causes:\n> uniform does not exist,\n> compiler decided to ignore the uniform\n> total uniform component count was exceeded in the shader (check Shader::RetrieveInfo)\n> other error has occured that could not be caught."; \
		AE_ASSERT_FALSE(error_info); \
		LogError("[Aether] " + error_info, true); \
	}

#define AE_SHADER_UNIFORM_MATRIX_ASSERTS \
	AE_DEBUG_ONLY( \
		auto cache_it = m_shader->m_uniform_array_sizes.find(name); \
		 \
		/* matrix array */ \
		if (cache_it != m_shader->m_uniform_array_sizes.end()) { \
			AE_ASSERT(array_length != 0, "Could not set uniform matrix array, array_length equals zero"); \
			AE_ASSERT(array_index + array_length <= cache_it->second, "Could not set uniform matrix array, array index + array_length out of bounds"); \
		} \
		/* single matrix */ \
		else { \
			AE_ASSERT(array_index == 0 && array_length == 1, "Could not set uniform matrix array, it is not an array"); \
		} \
	)

#define AE_SHADER_UNIFORM_ARRAY_ASSERTS \
	AE_DEBUG_ONLY( \
		auto cache_it = m_shader->m_uniform_array_sizes.find(name); \
		AE_ASSERT(cache_it != m_shader->m_uniform_array_sizes.end(), "Could not set uniform array, such array does not exist"); \
		AE_ASSERT(array_length != 0, "Could not set uniform array, array_length equals zero"); \
		AE_ASSERT(array_index + array_length <= cache_it->second, "Could not set uniform array, array index + array_length out of bounds"); \
	)

#define AE_SHADER_UNIFORM_ASSERTS \
	AE_SHADER_UNIFORM_ASSERT_LOAD; \
	AE_SHADER_UNIFORM_ASSERT_WARNING_BIND; \
	AE_SHADER_UNIFORM_ASSERT_FIND

namespace ae {
	namespace internal {
		namespace shader {

			// Retrieve Information //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			size_t RetrieveInformationFunctions::MaxTextureSlotsFragment() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &value));
				return static_cast<size_t>(value);
			}
			size_t RetrieveInformationFunctions::MaxUniformLocations() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &value));
				return static_cast<size_t>(value);
			}
			size_t RetrieveInformationFunctions::MaxUniformComponentsVertex() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &value));
				return static_cast<size_t>(value);
			}
			size_t RetrieveInformationFunctions::MaxUniformComponentsFragment() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &value));
				return static_cast<size_t>(value);
			}
			size_t RetrieveInformationFunctions::MaxUniformComponentsGeometry() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &value));
				return static_cast<size_t>(value);
			}
			size_t RetrieveInformationFunctions::MaxUniformComponentsTessControl() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_TESS_CONTROL_UNIFORM_COMPONENTS, &value));
				return static_cast<size_t>(value);
			}
			size_t RetrieveInformationFunctions::MaxUniformComponentsTessEval() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_TESS_EVALUATION_UNIFORM_COMPONENTS, &value));
				return static_cast<size_t>(value);
			}
			size_t RetrieveInformationFunctions::MaxUniformComponentsCompute() {
				GLint value;
				AE_GL_LOG(glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &value));
				return static_cast<size_t>(value);
			}

			// Uniforms //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			SetUniformFunctions::SetUniformFunctions(ae::Shader* shader) : m_shader(shader) {}

			// Texture
			void SetUniformFunctions::Sampler2D(const std::string& name, size_t slot) const {
				AE_SHADER_UNIFORM_TEXTURE_BIND_ASSERT(slot);
				Int(name, slot);
			}

			// Floats
			void SetUniformFunctions::Float(const std::string& name, float value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform1f(m_shader->m_uniform_locations.at(name), value));
			}
			void SetUniformFunctions::Vec2f(const std::string& name, const Vector2f& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform2f(m_shader->m_uniform_locations.at(name), value.x, value.y));
			}
			void SetUniformFunctions::Vec3f(const std::string& name, const Vector3f& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform3f(m_shader->m_uniform_locations.at(name), value.x, value.y, value.z));
			}
			void SetUniformFunctions::Vec4f(const std::string& name, const Vector4f& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform4f(m_shader->m_uniform_locations.at(name), value.x, value.y, value.z, value.w));
			}
			void SetUniformFunctions::Vec4f(const std::string& name, const Color& color) const {
				Vec4f(name, color.GetNormalized());
			}

			// Ints
			void SetUniformFunctions::Int(const std::string& name, int value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform1i(m_shader->m_uniform_locations.at(name), value));
			}
			void SetUniformFunctions::Vec2i(const std::string& name, const Vector2i& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform2i(m_shader->m_uniform_locations.at(name), value.x, value.y));
			}
			void SetUniformFunctions::Vec3i(const std::string& name, const Vector3i& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform3i(m_shader->m_uniform_locations.at(name), value.x, value.y, value.z));
			}
			void SetUniformFunctions::Vec4i(const std::string& name, const Vector4i& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform4i(m_shader->m_uniform_locations.at(name), value.x, value.y, value.z, value.w));
			}

			// Unsigned Ints
			void SetUniformFunctions::UnsignedInt(const std::string& name, unsigned int value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform1ui(m_shader->m_uniform_locations.at(name), value));
			}
			void SetUniformFunctions::Vec2ui(const std::string& name, const Vector2ui& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform2ui(m_shader->m_uniform_locations.at(name), value.x, value.y));
			}
			void SetUniformFunctions::Vec3ui(const std::string& name, const Vector3ui& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform3ui(m_shader->m_uniform_locations.at(name), value.x, value.y, value.z));
			}
			void SetUniformFunctions::Vec4ui(const std::string& name, const Vector4ui& value) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_GL_LOG(glUniform4ui(m_shader->m_uniform_locations.at(name), value.x, value.y, value.z, value.w));
			}

			// Texture Arrays
			void SetUniformFunctions::Sampler2DArray(const std::string& name, const size_t* slots, size_t array_index, size_t array_length) const {
				AE_DEBUG_ONLY(
					for (size_t i(array_index); i < array_length; i++) {
						AE_SHADER_UNIFORM_TEXTURE_BIND_ASSERT(slots[i]);
					}
				);
				IntArray(name, reinterpret_cast<const int*>(slots), array_index, array_length); // first byte of slots can be ignored
			}

			// Float Arrays
			void SetUniformFunctions::FloatArray(const std::string& name, const float* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform1fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, value));
			}
			void SetUniformFunctions::Vec2fArray(const std::string& name, const Vector2f* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform2fv(m_shader->m_uniform_locations.at(name) + array_index + array_index, array_length, &value[0].x));
			}
			void SetUniformFunctions::Vec3fArray(const std::string& name, const Vector3f* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform3fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}
			void SetUniformFunctions::Vec4fArray(const std::string& name, const Vector4f* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform4fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}

			void SetUniformFunctions::Vec4fArray(const std::string& name, const Color* value, size_t array_index, size_t array_length) const {
				Vector4f* normalized_value = new Vector4f[array_length];

				for (size_t i = 0; i < array_length; i++)
					normalized_value[i] = value[i].GetNormalized();

				Vec4fArray(name, normalized_value, array_index, array_length);
				delete[] normalized_value;
			}

			// Int Arrays
			void SetUniformFunctions::IntArray(const std::string& name, const int* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform1iv(m_shader->m_uniform_locations.at(name) + array_index, array_length, value));
			}
			void SetUniformFunctions::Vec2iArray(const std::string& name, const Vector2i* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform2iv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}
			void SetUniformFunctions::Vec3iArray(const std::string& name, const Vector3i* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform3iv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}
			void SetUniformFunctions::Vec4iArray(const std::string& name, const Vector4i* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform4iv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}

			// Unsigned Int Arrays
			void SetUniformFunctions::UnsignedIntArray(const std::string& name, const unsigned int* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform1uiv(m_shader->m_uniform_locations.at(name) + array_index, array_length, value));
			}
			void SetUniformFunctions::Vec2uiArray(const std::string& name, const Vector2ui* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform2uiv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}
			void SetUniformFunctions::Vec3uiArray(const std::string& name, const Vector3ui* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform3uiv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}
			void SetUniformFunctions::Vec4uiArray(const std::string& name, const Vector4ui* value, size_t array_index, size_t array_length) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_ARRAY_ASSERTS;
				AE_GL_LOG(glUniform4uiv(m_shader->m_uniform_locations.at(name) + array_index, array_length, &value[0].x));
			}

			// Matrices
			void SetUniformFunctions::Mat2x2(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix2fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}
			void SetUniformFunctions::Mat2x3(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix2x3fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}
			void SetUniformFunctions::Mat2x4(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix2x4fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}

			void SetUniformFunctions::Mat3x2(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix3x2fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}
			void SetUniformFunctions::Mat3x3(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix3fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}
			void SetUniformFunctions::Mat3x4(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix3x4fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}

			void SetUniformFunctions::Mat4x2(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix4x2fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}
			void SetUniformFunctions::Mat4x3(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix4x3fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}
			void SetUniformFunctions::Mat4x4(const std::string& name, const float* mat, size_t array_index, size_t array_length, bool transpose) const {
				AE_SHADER_UNIFORM_ASSERTS;
				AE_SHADER_UNIFORM_MATRIX_ASSERTS;
				AE_GL_LOG(glUniformMatrix4fv(m_shader->m_uniform_locations.at(name) + array_index, array_length, transpose, mat));
			}

		} // shader
	}
}
