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

#include "Color.hpp"
#include "../System/Vector2.hpp"
#include "../System/Vector3.hpp"
#include "../System/Vector4.hpp"

#include <string>

namespace ae {
	class Shader;

	namespace internal {
		namespace shader {

			// Retrieve Information
			class RetrieveInformationFunctions {
				friend class ae::Shader;

			public:
				static size_t MaxTextureSlotsFragment();
				static size_t MaxUniformLocations();

				static size_t MaxUniformComponentsVertex();
				static size_t MaxUniformComponentsFragment();
				static size_t MaxUniformComponentsGeometry();
				static size_t MaxUniformComponentsTessControl();
				static size_t MaxUniformComponentsTessEval();
				static size_t MaxUniformComponentsCompute();

			private:
				RetrieveInformationFunctions() = default;
				RetrieveInformationFunctions(const RetrieveInformationFunctions&) = default;
				RetrieveInformationFunctions(RetrieveInformationFunctions&&) = default;
			};

			// SetUniforms
			class SetUniformFunctions {
				friend class ae::Shader;

			public:

				// Texture
				void Sampler2D(const std::string& name, size_t slot) const;

				// Floats
				void Float(const std::string& name, float           value) const;
				void Vec2f(const std::string& name, const Vector2f& value) const;
				void Vec3f(const std::string& name, const Vector3f& value) const;
				void Vec4f(const std::string& name, const Vector4f& value) const;

				void Vec4f(const std::string& name, const Color& value) const;

				// Ints
				void Int(const std::string& name, int             value) const;
				void Vec2i(const std::string& name, const Vector2i& value) const;
				void Vec3i(const std::string& name, const Vector3i& value) const;
				void Vec4i(const std::string& name, const Vector4i& value) const;

				// Unsigned Ints
				void UnsignedInt(const std::string& name, unsigned int     value) const;
				void Vec2ui(const std::string& name, const Vector2ui& value) const;
				void Vec3ui(const std::string& name, const Vector3ui& value) const;
				void Vec4ui(const std::string& name, const Vector4ui& value) const;

				// Float Arrays
				void FloatArray(const std::string& name, const float* value, size_t array_index, size_t array_length) const;
				void Vec2fArray(const std::string& name, const Vector2f* value, size_t array_index, size_t array_length) const;
				void Vec3fArray(const std::string& name, const Vector3f* value, size_t array_index, size_t array_length) const;
				void Vec4fArray(const std::string& name, const Vector4f* value, size_t array_index, size_t array_length) const;

				void Vec4fArray(const std::string& name, const Color* value, size_t array_index, size_t array_length) const;

				// Texture Array
				void Sampler2DArray(const std::string& name, const size_t* slots, size_t array_index, size_t array_length) const;

				// Int Arrays
				void IntArray(const std::string& name, const int* value, size_t array_index, size_t array_length) const;
				void Vec2iArray(const std::string& name, const Vector2i* value, size_t array_index, size_t array_length) const;
				void Vec3iArray(const std::string& name, const Vector3i* value, size_t array_index, size_t array_length) const;
				void Vec4iArray(const std::string& name, const Vector4i* value, size_t array_index, size_t array_length) const;

				// Unsigned Int Arrays
				void UnsignedIntArray(const std::string& name, const unsigned int* value, size_t array_index, size_t array_length) const;
				void Vec2uiArray(const std::string& name, const Vector2ui* value, size_t array_index, size_t array_length) const;
				void Vec3uiArray(const std::string& name, const Vector3ui* value, size_t array_index, size_t array_length) const;
				void Vec4uiArray(const std::string& name, const Vector4ui* value, size_t array_index, size_t array_length) const;

				// Matrices
				void Mat2x2(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;
				void Mat2x3(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;
				void Mat2x4(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;

				void Mat3x2(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;
				void Mat3x3(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;
				void Mat3x4(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;

				void Mat4x2(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;
				void Mat4x3(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;
				void Mat4x4(const std::string& name, const float* value, size_t array_index = 0, size_t array_length = 1, bool transpose = false) const;

			private:
				Shader* m_shader;

				SetUniformFunctions(Shader* shader);
				SetUniformFunctions(const SetUniformFunctions&) = default;
				SetUniformFunctions(SetUniformFunctions&&) = default;
			};
		}
	}
}