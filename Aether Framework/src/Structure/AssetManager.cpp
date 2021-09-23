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

#include "Structure/AssetManager.hpp"
#include "AssetManagerDefaultAssetsData.cpp"

#include "Core/Preprocessor.hpp"
#include "System/LogError.hpp"

#include <array>

#define AE_GET_ASSET(name, container, default_asset, string_type) \
	auto pos = container.find(name); \
 \
	if (pos != container.end()) { \
		return *(pos->second); \
	} \
 \
	AE_WARNING(string_type << " '" << name << "' does not exists"); \
	return default_asset; \

#define AE_REMOVE_ASSET(name, container, string_type) \
	auto pos = container.find(name); \
 \
	if (pos != container.end()) { \
		container.erase(pos); \
		return; \
	} \
	AE_WARNING(string_type << " '" << name << "' does not exists");

#define AE_CLEAR_ASSETS(container) \
	container.clear();

namespace ae {

	internal::AssetManagerType AssetManager = internal::CreateStructure<internal::AssetManagerType>();
	internal::DefaultAssetsType* DefaultAssets;

	namespace internal {

		void AssetManagerType::Initialize() {
			DefaultAssets = new DefaultAssetsType();
		}
		void AssetManagerType::Terminate() {
			Clear();
			delete DefaultAssets;
		}

		void AssetManagerType::Clear() {
			ClearShaders();
			ClearTextures();
			ClearFonts();
			ClearSoundBuffers();
		}

		// Shaders ////////////////////////////////////////////////////////////////////////////////////////////////////////////

		bool AssetManagerType::LoadShader(
			const std::string& name,
			Shader::LoadMode mode,
			const std::string& vertex,
			const std::string& fragment,
			const std::string& geometry,
			const std::string& tess_control,
			const std::string& tess_eval,
			const std::string& compute)
		{
			// Find if shader exists already
			if (m_shaders.find(name) != m_shaders.end()) {
				AE_WARNING("Shader '" << name << "' already exists");
				return false;
			}
			
			// Load
			Shader* shader = new Shader();
			bool success = shader->Load(mode, vertex, fragment, geometry, tess_control, tess_eval, compute);

			// Success
			if (success)
				m_shaders.emplace(name, shader);

			// Failure
			else {
				AE_WARNING("Could not load shader '" << name << '\'');

				LogError(
					"[Aether] Could not load shader '" + name + "' from:"
					+ "\nvertex:\n"          + ((vertex       != "") ? vertex       : "    not present")
					+ "\nfragment:\n"        + ((fragment     != "") ? fragment     : "    not present")
					+ "\ngeometry:\n"        + ((geometry     != "") ? geometry     : "    not present")
					+ "\ntess_control:\n"    + ((tess_control != "") ? tess_control : "    not present")
					+ "\ntess_evaluation:\n" + ((tess_eval    != "") ? tess_eval    : "    not present")
					+ "\ncompute:\n"         + ((compute      != "") ? compute      : "    not present"),
					false);

				delete shader;
			}

			// Return
			return success;

		}

		Shader& AssetManagerType::GetShader(const std::string& name) const {
			AE_GET_ASSET(name, m_shaders, GetDefaultShader(), "Shader");
		}

		void AssetManagerType::RemoveShader(const std::string& name) {
			AE_REMOVE_ASSET(name, m_shaders, "Shader");
		}

		void AssetManagerType::ClearShaders() {
			AE_CLEAR_ASSETS(m_shaders);
		}

		Shader& AssetManagerType::GetDefaultShader() const {
			return DefaultAssets->random_shader;
		}

		// Textures ////////////////////////////////////////////////////////////////////////////////////////////////////////////

		bool AssetManagerType::LoadTexture(const std::string& name, const std::string& filename) {

			// Find if shader exists already
			if (m_textures.find(name) != m_textures.end()) {
				AE_WARNING("Texture '" << name << "' already exists");
				return false;
			}

			// Create new one
			Texture* texture = new Texture();

			// Success
			if (texture->LoadFromFile(filename)) {
				m_textures.emplace(name, texture);
				return true;
			}

			// Failure
			AE_WARNING("Could not load '" << name << "' texture, either file does not exist or format is not supported");
			LogError("[Aether] Could not load texture '" + name + "' from file '" + filename + "'", false);
			delete texture;
			return false;
		}

		bool AssetManagerType::LoadTexture(const std::string& name, const TextureCanvas& texture_canvas) {

			// Find if exists already
			if (m_textures.find(name) != m_textures.end()) {
				AE_WARNING("Texture '" << name << "' already exists");
				return false;
			}

			// Create new one
			Texture* texture = new Texture();

			texture->LoadFromCanvas(texture_canvas);
			m_textures.emplace(name, texture);

			return true;
		}

		Texture& AssetManagerType::GetTexture(const std::string& name) const {
			AE_GET_ASSET(name, m_textures, GetDefaultTexture(), "Texture");
		}

		void AssetManagerType::RemoveTexture(const std::string& name) {
			AE_REMOVE_ASSET(name, m_textures, "Texture");
		}

		void AssetManagerType::ClearTextures() {
			AE_CLEAR_ASSETS(m_textures);
		}

		Texture& AssetManagerType::GetDefaultTexture() const {
			return DefaultAssets->checkboard_texture;
		}

		// Fonts ////////////////////////////////////////////////////////////////////////////////////////////////////////////

		bool AssetManagerType::LoadFont(const std::string& name, const std::string& filename) {

			// Find if exists already
			if (m_fonts.find(name) != m_fonts.end()) {
				AE_WARNING("Font '" << name << "' already exists");
				return false;
			}

			// Create new one
			Font* font = new Font();

			// Success
			if (font->LoadFromFile(filename)) {
				m_fonts.emplace(name, font);
				return true;
			}

			// Failure
			AE_WARNING("Could not load '" << name << "' font, either file does not exist or format is not supported");
			LogError("[Aether] Could not load font '" + name + "' from file '" + filename + "'", false);
			delete font;
			return false;
		}
		Font& AssetManagerType::GetFont(const std::string& name) const {
			AE_GET_ASSET(name, m_fonts, GetDefaultFont(), "Font");
		}
		void AssetManagerType::RemoveFont(const std::string& name) {
			AE_REMOVE_ASSET(name, m_fonts, "Font");
		}
		void AssetManagerType::ClearFonts() {
			AE_CLEAR_ASSETS(m_fonts);
		}
		Font& AssetManagerType::GetDefaultFont() const {
			return DefaultAssets->inter_regular_font;
		}

		// SoundBuffers /////////////////////////////////////////////////////////////////////////////////////////////////////////////

		bool AssetManagerType::LoadSoundBuffer(const std::string& name, const std::string& filename, bool cache_samples) {
			
			// Find if exists already
			if (m_sound_buffers.find(name) != m_sound_buffers.end()) {
				AE_WARNING("SoundBuffer '" << name << "' already exists");
				return false;
			}

			// Create new one
			SoundBuffer* sound_buffer = new SoundBuffer();

			// Success
			if (sound_buffer->LoadFromFile(filename, cache_samples)) {
				m_sound_buffers.emplace(name, sound_buffer);
				return true;
			}

			// Failure
			AE_WARNING("Could not load '" << name << "' soundbuffer, either file does not exist or format is not supported");
			LogError("[Aether] Could not load soundbuffer '" + name + "' from file '" + filename + "'", false);
			delete sound_buffer;
			return false;
		}
		bool AssetManagerType::LoadSoundBuffer(const std::string& name, const float* samples, size_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples) {
			
			// Find if exists already
			if (m_sound_buffers.find(name) != m_sound_buffers.end()) {
				AE_WARNING("SoundBuffer '" << name << "' already exists");
				return false;
			}

			// Create new one
			SoundBuffer* sound_buffer = new SoundBuffer();
			sound_buffer->LoadFromSamples(samples, sample_count, channel_count, sample_rate, cache_samples);

			m_sound_buffers.emplace(name, sound_buffer);
			return true;
		}
		bool AssetManagerType::LoadSoundBuffer(const std::string& name, const std::int16_t* samples, size_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples) {

			// Find if exists already
			if (m_sound_buffers.find(name) != m_sound_buffers.end()) {
				AE_WARNING("SoundBuffer '" << name << "' already exists");
				return false;
			}

			// Create new one
			SoundBuffer* sound_buffer = new SoundBuffer();
			sound_buffer->LoadFromSamples(samples, sample_count, channel_count, sample_rate, cache_samples);

			m_sound_buffers.emplace(name, sound_buffer);
			return true;
		}
		SoundBuffer& AssetManagerType::GetSoundBuffer(const std::string& name) const {
			AE_GET_ASSET(name, m_sound_buffers, GetDefaultSoundBuffer(), "SoundBuffer");
		}
		void AssetManagerType::RemoveSoundBuffer(const std::string& name) {
			AE_REMOVE_ASSET(name, m_sound_buffers, "SoundBuffer");
		}
		void AssetManagerType::ClearSoundBuffers() {
			AE_CLEAR_ASSETS(m_sound_buffers);
		}
		SoundBuffer& AssetManagerType::GetDefaultSoundBuffer() const {
			return DefaultAssets->water_splash_soundbuffer;
		}

		// default assets //////////////////////////////////////////////////////////////////////////////////////////////////////////////
		DefaultAssetsType::DefaultAssetsType() {

			random_shader.Load(
				Shader::LoadMode::FromSource,

				"#version 460 core \n\
					\n\
					in int gl_VertexID; \n\
					\n\
					void main() \n\
					{ \n\
						gl_Position = vec4(sin(gl_VertexID), sin(gl_VertexID * 2), 1.0, 1.0); \n\
					}",

				"#version 460 core \n\
					\n\
					layout(location = 0) out vec4 a_color; \n\
					\n\
					void main() \n\
					{ \n\
						a_color = vec4(0.3, 0.5, 0.8, 1.0); \n\
					}"
			);
			text_shader.Load(
				Shader::LoadMode::FromSource,

				"#version 460 core \n\
					 \n\
					layout(location = 0) in vec2 a_position; \n\
					layout(location = 1) in vec2 a_texcoords; \n\
					 \n\
					out vec2 v_texcoords; \n\
					 \n\
					uniform mat3 u_mvp; \n\
					 \n\
					void main() \n\
					{ \n\
						gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0); \n\
						v_texcoords = a_texcoords; \n\
					}",

				"#version 460 core \n\
					 \n\
					layout(location = 0) out vec4 a_color; \n\
					 \n\
					in vec4 v_color; \n\
					in vec2 v_texcoords; \n\
					 \n\
					uniform sampler2D u_texture; \n\
					uniform vec4 u_color; \n\
					 \n\
					void main() \n\
					{  \n\
						\/\/ normalize pixel texture coords  \n\
						vec2 normalized_coords = v_texcoords / vec2(textureSize(u_texture, 0)); \n\
						\n\
						\/\/ output color  \n\
						float alpha = texture(u_texture, normalized_coords).r; \n\
						a_color = vec4(u_color.rgb, u_color.a * alpha); \n\
					}"
			);

			sprite_shader.Load(
				Shader::LoadMode::FromSource,

				"#version 460 core \n\
					\n\
					layout(location = 0) in vec2 a_position; \n\
					layout(location = 1) in vec2 a_texcoords; \n\
					\n\
					out vec2 v_texcoords; \n\
					\n\
					uniform mat3 u_mvp; \n\
					\n\
					void main() \n\
					{ \n\
						gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0); \n\
						v_texcoords = a_texcoords; \n\
					}",

				"#version 460 core \n\
					\n\
					layout(location = 0) out vec4 a_color; \n\
					\n\
					in vec4 v_color; \n\
					in vec2 v_texcoords; \n\
					\n\
					uniform sampler2D u_texture; \n\
					uniform vec4 u_color; \n\
					\n\
					void main() \n\
					{ \n\
						\/\/ normalize pixel texture coords \n\
						vec2 normalized_coords = v_texcoords / vec2(textureSize(u_texture, 0)); \n\
					\n\
						\/\/ output color \n\
						a_color = u_color * texture(u_texture, normalized_coords); \n\
					}"
			);

			color_shader.Load(
				Shader::LoadMode::FromSource,

				"#version 460 core \n\
					\n\
					layout(location = 0) in vec2 a_position; \n\
					\n\
					uniform mat3 u_mvp; \n\
					\n\
					void main() \n\
					{ \n\
						gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0); \n\
					}",

				"#version 460 core \n\
					 \n\
					layout(location = 0) out vec4 a_color; \n\
					 \n\
					in vec4 v_color; \n\
					 \n\
					uniform vec4 u_color; \n\
					 \n\
					void main() \n\
					{  \n\
						a_color = u_color; \n\
					}"
			);

			framesprite_shader.Load(
				Shader::LoadMode::FromSource,

				"#version 460 core \n\
					\n\
					layout(location = 0) in vec2 a_position; \n\
					layout(location = 1) in vec2 a_texcoords; \n\
					\n\
					out vec2 v_texcoords; \n\
					\n\
					uniform mat3 u_mvp; \n\
					\n\
					void main() \n\
					{ \n\
						gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0); \n\
						v_texcoords = a_texcoords; \n\
					}",

				"#version 460 core \n\
					\n\
					layout(location = 0) out vec4 a_color; \n\
					\n\
					in vec2 v_texcoords; \n\
					\n\
					uniform sampler2D u_texture; \n\
					uniform vec4 u_color; \n\
					\n\
					void main() \n\
					{ \n\
						\/\/ output color \n\
						a_color = u_color * texture(u_texture, v_texcoords); \n\
					}"
			);

			// instance shaders
			constexpr size_t mat3_comp_count = 12, vec4_comp_count = 4;
			instanced_sprite_max_draws_per_call = static_cast<size_t>((Shader::RetrieveInfo.MaxUniformComponentsVertex() - mat3_comp_count) / (mat3_comp_count + vec4_comp_count));

			instanced_sprite_shader.Load(
				Shader::LoadMode::FromSource,

				"#version 460 core \n\
					 \n\
					layout(location = 0) in vec2 a_position; \n\
					layout(location = 1) in vec2 a_texcoords; \n\
					 \n\
					out vec2 v_texcoords; \n\
					out vec4 v_color; \n\
					 \n\
					uniform vec4 u_colors[" + std::to_string(instanced_sprite_max_draws_per_call) + "]; \n\
					uniform mat3 u_models[" + std::to_string(instanced_sprite_max_draws_per_call) + "]; \n\
					uniform mat3 u_vp; \n\
					 \n\
					void main() \n\
					{ \n\
						gl_Position = vec4(u_vp * u_models[gl_InstanceID] * vec3(a_position, 1.0), 1.0); \n\
						v_texcoords = a_texcoords; \n\
						v_color = u_colors[gl_InstanceID]; \n\
					}",

				"#version 460 core \n\
					 \n\
					layout(location = 0) out vec4 a_color; \n\
					 \n\
					in vec2 v_texcoords; \n\
					in vec4 v_color; \n\
					 \n\
					uniform sampler2D u_texture; \n\
					 \n\
					void main() \n\
					{  \n\
						vec2 normalized_coords = v_texcoords / vec2(textureSize(u_texture, 0)); \n\
						a_color = texture(u_texture, normalized_coords) * v_color; \n\
					}"
			);

			// batch shaders
			constexpr size_t uint_comp_count = 1;
			batch_sprite_max_draws_per_call = static_cast<size_t>((Shader::RetrieveInfo.MaxUniformComponentsVertex() - mat3_comp_count - uint_comp_count) / mat3_comp_count);

			batch_sprite_shader.Load(
				Shader::LoadMode::FromSource,

				"#version 460 core \n\
					 \n\
					layout(location = 0) in vec2 a_position; \n\
					layout(location = 1) in vec2 a_texcoords; \n\
					layout(location = 2) in vec4 a_color; \n\
					 \n\
					out vec2 v_texcoords; \n\
					out vec4 v_color; \n\
					 \n\
					uniform uint u_rendered_batches; \n\
					uniform mat3 u_models[ " + std::to_string(batch_sprite_max_draws_per_call) + " ]; \n\
					uniform mat3 u_vp; \n\
					 \n\
					void main() \n\
					{ \n\
						gl_Position = vec4(u_vp * u_models[gl_VertexID/4 - u_rendered_batches] * vec3(a_position, 1.0), 1.0); \n\
						v_texcoords = a_texcoords; \n\
						v_color = a_color; \n\
					}",

				"#version 460 core \n\
					 \n\
					layout(location = 0) out vec4 a_color; \n\
					 \n\
					in vec2 v_texcoords; \n\
					in vec4 v_color; \n\
					 \n\
					uniform sampler2D u_texture; \n\
					 \n\
					void main() \n\
					{ \n\
						vec2 normalized_coords = v_texcoords / vec2(textureSize(u_texture, 0)); \n\
						a_color = texture(u_texture, normalized_coords) * v_color; \n\
					}"
			);

			// checkboard
			TextureCanvas canvas;
			canvas.Create(Vector2ui(2, 2), Color(160, 140, 140));
			canvas.Px(0, 1) = canvas.Px(0, 1).GetInverse();
			canvas.Px(1, 0) = canvas.Px(1, 0).GetInverse();

			checkboard_texture.LoadFromCanvas(canvas);

			// white pixel
			Color white = Color::White;
			white_pixel_texture.LoadFromData(&white, Vector2ui(1, 1));

			// default font
			inter_regular_font.LoadFromData(c_default_font_buffer, sizeof(c_default_font_buffer));

			// default soundbuffer
			water_splash_soundbuffer.LoadFromSamples(
				c_default_soundbuffer_buffer, sizeof(c_default_soundbuffer_buffer) / sizeof(std::int16_t),
				c_default_soundbuffer_channel_count, c_default_soundbuffer_sample_rate,
				false
			);
		}
	}
}
