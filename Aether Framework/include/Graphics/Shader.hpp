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

//////////////////////////////////////////////////////////////////////////////////////////////////////
/// Shader
///
/// Loading and Compiling:
///		Load(mode, modules...) function is used to load the shader, 
///		it takes as parameters load mode, which could be LoadMode::FromSource or LoadMode::FromFile
///		and module strings that are either source code or filenames depending on the mode.
///		All compilation errors are shown in the console.
/// 
/// Using:
///		To use a shader simply Bind() it, Unbind() is also available.
/// 
/// Uniforms:
///		SetUniform object contains all neccessary functions needed for setting uniforms.
///		Before setting an uniform, shader has to be bound.
/// 
///		To set a sampler2D (texture) uniform use SetUniform.Sampler2D(name, slot), 
///		where the slot is the unit/texture slot to which the ae::Texture is bound.
/// 
///		SetUniform.Vec4f(color) and SetUniform.Vec4fArray(colors) normalizes colors to Vector4f.
/// 
/// Information: 
///		RetrieveInfo static object is used for retrieving devices shader limits, such as uniform component count or texture unit limit for fragment shader.
/// 
///		Component count for glsl types:
///			Type:			       count:  count in array:
/// 
///			float, int, or bool	   1	   4
///			vec2, ivec2, or bvec2  2	   4
///			vec3, ivec3, or bvec3  4	   4
///			vec4, ivec4, or bvec4  4	   4
///			mat2	               4	   4
///			mat3	               12	   12
///			mat4	               16	   16
/// 
///		table source: https://people.freedesktop.org/~idr/OpenGL_tutorials/04-uniforms.html#uniform-components
///		
//////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "ShaderFunctions.hpp"
#include "../Core/Preprocessor.hpp"

#include <string>
#include <unordered_map>

namespace ae {

	class Shader;

	bool operator==(const Shader& left, const Shader& right);
	bool operator!=(const Shader& left, const Shader& right);

	class Shader {
		friend bool ae::operator==(const Shader& left, const Shader& right);
		friend bool ae::operator!=(const Shader& left, const Shader& right);

		friend class internal::shader::SetUniformFunctions;

	public:
		enum class LoadMode {
			FromFile,
			FromSource
		};

		Shader();
		Shader(Shader&& move) noexcept;
		Shader(const Shader&) = delete;
		~Shader();

		bool Load(
			LoadMode mode,
			const std::string& vertex,
			const std::string& fragment,
			const std::string& geometry     = "",
			const std::string& tess_control = "",
			const std::string& tess_eval    = "",
			const std::string& compute      = "");

		bool WasLoaded() const;

		void Bind() const;
		void Unbind() const;

		static ae::internal::shader::RetrieveInformationFunctions RetrieveInfo;
		ae::internal::shader::SetUniformFunctions SetUniform;

	private:
		std::uint32_t m_shader_id = 0;
		AE_DEBUG_ONLY(static std::uint32_t s_bound_shader_id);

		std::unordered_map<std::string, std::int32_t> m_uniform_locations;

		using DebugUniformArraySizeMap = std::unordered_map<std::string, size_t>;
		AE_DEBUG_ONLY(DebugUniformArraySizeMap m_uniform_array_sizes);

		std::uint32_t LoadModuleFromFile(const std::string& filename, std::int32_t shader_type);
		std::uint32_t LoadModuleFromSource(const std::string& source, std::int32_t shader_type);
		void Validate(std::uint32_t* module_ids, size_t array_length);
		void FreeModules(std::uint32_t* module_ids, size_t array_length);

		std::uint32_t CompileModule(const std::string& source, std::int32_t shader_type);
		void LocateUniforms();
	};
}
