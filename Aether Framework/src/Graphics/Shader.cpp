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
#include "Core/OpenGLCalls.hpp"
#include "System/LogError.hpp"

#include <fstream>

#include <glad/glad.h>

namespace ae {

	AE_DEBUG_ONLY(std::uint32_t Shader::s_bound_shader_id);
	ae::internal::shader::RetrieveInformationFunctions Shader::RetrieveInfo;


	Shader::Shader() : SetUniform(this) {}

	Shader::Shader(Shader&& move) noexcept 
		:SetUniform(this)
	{
		m_shader_id = move.m_shader_id;
		m_uniform_locations = std::move(move.m_uniform_locations);

		AE_DEBUG_ONLY(
			m_uniform_array_sizes = std::move(move.m_uniform_array_sizes);
		)

		move.m_shader_id = 0;
	}

	Shader::~Shader() {
		AE_GL_LOG(glDeleteProgram(m_shader_id));
	}

	// Loading //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	bool Shader::WasLoaded() const { return (m_shader_id != 0); }

	bool Shader::Load(
		LoadMode mode,
		const std::string& vertex,
		const std::string& fragment,
		const std::string& geometry,
		const std::string& tess_control,
		const std::string& tess_eval,
		const std::string& compute)
	{

		// Was loaded
		AE_ASSERT(m_shader_id == 0, "Shader was already loaded");

		// Create
		AE_GL_LOG(m_shader_id = glCreateProgram());

		// Cover shader filename and type in a nice struct to iterate later
		constexpr size_t shader_types = 6;

		struct ShaderInfo {
			std::string input;
			std::int32_t shader_type;
		}
		shader_info[shader_types] =
		{
			{ vertex,       GL_VERTEX_SHADER },
			{ fragment,     GL_FRAGMENT_SHADER },
			{ geometry,     GL_GEOMETRY_SHADER },
			{ tess_control, GL_TESS_CONTROL_SHADER },
			{ tess_eval,    GL_TESS_EVALUATION_SHADER },
			{ compute,      GL_COMPUTE_SHADER }
		};

		// Create the shader, load the modules
		bool loading_success = true;
		std::uint32_t loaded_modules[shader_types];
		size_t loaded_modules_count = 0;

		for (size_t i = 0; i < shader_types; i++) {
			if (shader_info[i].input != "") {
				std::uint32_t module_id;

				if (mode == LoadMode::FromFile)
					module_id = LoadModuleFromFile(shader_info[i].input, shader_info[i].shader_type);
				else
					module_id = LoadModuleFromSource(shader_info[i].input, shader_info[i].shader_type);

				if (module_id == 0) {
					AE_DEBUG_ONLY(
						if (mode == LoadMode::FromFile) {
							AE_WARNING("Shader: Could not load file '" << shader_info[i].input << "'");
						}
						else {
							AE_WARNING("Shader: Could not load source '" << shader_info[i].input << "'");
						}
					);

					loading_success = false;
					break;
				}
				else {
					loaded_modules[loaded_modules_count] = module_id;
					loaded_modules_count++;
				}
			}
		}

		AE_ASSERT_WARNING((!loading_success || loaded_modules_count != 0), "Shader: No module present");

		// Success - validate(create program & free modules)
		if (loading_success && loaded_modules_count != 0) {
			Validate(loaded_modules, loaded_modules_count);
			return true;
		}

		// Failure - clean up
		FreeModules(loaded_modules, loaded_modules_count);
		AE_GL_LOG(glDeleteProgram(m_shader_id));
		m_shader_id = 0;
		return false;
	}

	std::uint32_t Shader::LoadModuleFromFile(const std::string& filename, std::int32_t shader_type) {

		// Load source
		std::ifstream file(filename, std::ios::in);
		if (!file.good()) 
			return 0;

		std::string source, line;
		while (std::getline(file, line))
			source += line + '\n';
		
		file.close();

		// Load from source
		return LoadModuleFromSource(source, shader_type);
		
	}

	std::uint32_t Shader::LoadModuleFromSource(const std::string& source, std::int32_t shader_type) {
		
		// Compile the source
		std::uint32_t module_id = CompileModule(source, shader_type);
		if (module_id != 0) {
			AE_GL_LOG(glAttachShader(m_shader_id, module_id));
		}

		return module_id;
	}

	void Shader::LocateUniforms() {

		// Retrieve maximum array_length
		GLint max_uniform_length;
		AE_GL_LOG(glGetProgramiv(m_shader_id, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_uniform_length));

		// Retrieve uniform count
		GLint uniform_count;
		AE_GL_LOG(glGetProgramiv(m_shader_id, GL_ACTIVE_UNIFORMS, &uniform_count));
		
		// Retrieve names and locations
		GLchar* name = new GLchar[max_uniform_length];
		GLint location;
		GLsizei length;

		GLint size;
		GLenum type;

		for (GLuint i(0); i < uniform_count; i++) {
			AE_GL_LOG(glGetActiveUniform(m_shader_id, i, max_uniform_length, &length, &size, &type, name));
			AE_GL_LOG(location = glGetUniformLocation(m_shader_id, name));

			if (strstr(name, "[0]")) {
				std::string array_name = name;
				array_name.erase(length - 3);

				m_uniform_locations.emplace(array_name, location);
				AE_DEBUG_ONLY(m_uniform_array_sizes.emplace(array_name, size));
			}
			else
				m_uniform_locations.emplace(name, location);
		}

		delete[] name;
	}

	std::uint32_t Shader::CompileModule(const std::string& source, std::int32_t shader_type) {
		AE_GL_LOG(std::uint32_t module_id = glCreateShader(shader_type));
		const char* data = source.c_str();
		
		AE_GL_LOG(glShaderSource(module_id, 1, &data, nullptr));
		AE_GL_LOG(glCompileShader(module_id));

		GLint result;
		AE_GL_LOG(glGetShaderiv(module_id, GL_COMPILE_STATUS, &result));
		if (result == GL_FALSE) {
			AE_DEBUG_ONLY(
				GLint length;
				AE_GL_LOG(glGetShaderiv(module_id, GL_INFO_LOG_LENGTH, &length));

				char* message = new char[length];
				AE_GL_LOG(glGetShaderInfoLog(module_id, length, &length, message));

				std::string shader_str;

				if      (shader_type == GL_VERTEX_SHADER)          shader_str = "vertex";
				else if (shader_type == GL_FRAGMENT_SHADER)        shader_str = "fragment";
				else if (shader_type == GL_GEOMETRY_SHADER)        shader_str = "geometry";
				else if (shader_type == GL_TESS_CONTROL_SHADER)    shader_str = "tesselation control";
				else if (shader_type == GL_TESS_EVALUATION_SHADER) shader_str = "tesselation evaluation";
				else if (shader_type == GL_COMPUTE_SHADER)         shader_str = "compute";

				AE_WARNING("Failed to compile " << shader_str << " shader\n" << message);
				delete[] message;
			)
			AE_GL_LOG(glDeleteShader(module_id));
			return 0;
		}
		return module_id;
	}

	void Shader::Validate(std::uint32_t* module_ids, size_t array_length) {
		AE_GL_LOG(glLinkProgram(m_shader_id));
		AE_GL_LOG(glValidateProgram(m_shader_id));
		FreeModules(module_ids, array_length);
		LocateUniforms();
	}

	void Shader::FreeModules(std::uint32_t* module_ids, size_t array_length) {
		for (size_t i = 0; i < array_length; i++) {
			AE_GL_LOG(glDeleteShader(module_ids[i]));
		}
	}

	// Usage //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		 
	void Shader::Bind() const {
		if (m_shader_id == 0) {
			AE_ASSERT_FALSE("Could not bind shader, it has not been loaded yet");
			LogError("Could not bind shader, it has not been loaded yet. Perhaps the file missing?", true);
			std::exit(EXIT_FAILURE);
		}
		AE_GL_LOG(glUseProgram(m_shader_id));
		AE_DEBUG_ONLY(s_bound_shader_id = m_shader_id);
	}
	void Shader::Unbind() const {
		AE_GL_LOG(glUseProgram(0));
		AE_DEBUG_ONLY(s_bound_shader_id = 0);
	}

	bool operator==(const Shader& left, const Shader& right) {
		return (left.m_shader_id == right.m_shader_id);
	}
	bool operator!=(const Shader& left, const Shader& right) {
		return (left.m_shader_id != right.m_shader_id);
	}
}
