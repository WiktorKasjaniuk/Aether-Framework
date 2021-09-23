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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// AssetManagerType
///
///	General Idea:
///		Singleton that registers assets and stores them throughout the application.
///		All assets must exist, if they are being used by other classes.
/// 
///		Unloaded assets will assert, log error and close the application when program tries to use them,
///		thus if the file is missing in user's folder the application will crash.
///		To cope with that, AssetManager does return default assets, if they could not be loaded.
/// 
///		Renderers will ignore textures and fonts and Sound class will ignore soundbuffers if they're not loaded,
///		to make sure, the asset is loaded use WasLoaded() methods.
/// 
/// Managing Assets:
///		LoadAsset(register_name, parameters) returns false if could not load asset, otherwise loads it and returns true.
///		GetAsset(register_name) returns either the loaded asset or a default one if it was not registered before.
///		RemoveAsset(register_name) removes an asset.
///		ClearAssets() removes all assets.
///		GetDefaultAsset(asset) returns the default asset.
/// 
/// Default Assets:
///		ae::DefaultAssets pointer stores all default assets for both AssetManager and Renderers
///		(including calculated shader limits for batch & instanced rendering).
///		They are loaded from buffers at application initialization,
///		and deconstructed at termination.
/// 
///		Texture - 2 x 2 checkboard square,
///		Font - Inter (Regular) by Rasmus Andersson,
///		Sound - 16-bit stereo, water splash sound effect,
///		Shader - generates random shape on the screen:
///			Vertex Shader:	
///				#version 460 core 
/// 
///				void main() { 
///					gl_Position = vec4(sin(gl_VertexID), sin(gl_VertexID * 2), 1.0, 1.0); 
///				}",
///				
///			Fragment Shader
///				#version 460 core 
///				
///				layout(location = 0) out vec4 a_color; 
///				
///				void main() { 
///					a_color = vec4(0.3, 0.5, 0.8, 1.0); 
///				}
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Core/CreateStructure.hpp"

#include "../Graphics/Shader.hpp"
#include "../Graphics/Texture.hpp"
#include "../Graphics/TextureCanvas.hpp"
#include "../Graphics/Font.hpp"
#include "../Audio/SoundBuffer.hpp"

#include <unordered_map>
#include <string>
#include <memory>

namespace ae {

	namespace internal {

		class ApplicationType;

		class AssetManagerType {
			friend class ae::internal::ApplicationType;

			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<AssetManagerType>();

		public:
			~AssetManagerType() = default;

			void Clear();

			bool LoadShader(
				const std::string& name,
				Shader::LoadMode mode,
				const std::string& vertex,
				const std::string& fragment,
				const std::string& geometry     = "",
				const std::string& tess_control = "",
				const std::string& tess_eval    = "",
				const std::string& compute      = "");
			Shader& GetShader(const std::string& name) const;
			Shader& GetDefaultShader() const;
			void RemoveShader(const std::string& name);
			void ClearShaders();

			bool LoadTexture(const std::string& name, const std::string& filename);
			bool LoadTexture(const std::string& name, const TextureCanvas& texture_canvas);
			Texture& GetTexture(const std::string& name) const;
			Texture& GetDefaultTexture() const;
			void RemoveTexture(const std::string& name);
			void ClearTextures();

			bool LoadFont(const std::string& name, const std::string& filename);
			Font& GetFont(const std::string& name) const;
			Font& GetDefaultFont() const;
			void RemoveFont(const std::string& name);
			void ClearFonts();

			bool LoadSoundBuffer(const std::string& name, const std::string& filename, bool cache_samples = false);
			bool LoadSoundBuffer(const std::string& name, const float* samples, size_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples);
			bool LoadSoundBuffer(const std::string& name, const std::int16_t* samples, size_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples);
			SoundBuffer& GetSoundBuffer(const std::string& name) const;
			SoundBuffer& GetDefaultSoundBuffer() const;
			void RemoveSoundBuffer(const std::string& name);
			void ClearSoundBuffers();

		private:
			std::unordered_map<std::string, std::unique_ptr<Shader>>  m_shaders;
			std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
			std::unordered_map<std::string, std::unique_ptr<Font>> m_fonts;
			std::unordered_map<std::string, std::unique_ptr<SoundBuffer>> m_sound_buffers;

			AssetManagerType() = default;
			AssetManagerType(const AssetManagerType&) = delete;
			AssetManagerType(AssetManagerType&&) = delete;

			void Initialize();
			void Terminate();
		};

		struct DefaultAssetsType {
			DefaultAssetsType();
			~DefaultAssetsType() = default;

			Shader random_shader, text_shader, sprite_shader, color_shader, framesprite_shader;
			Texture checkboard_texture, white_pixel_texture;
			Font inter_regular_font;
			SoundBuffer water_splash_soundbuffer;
			Shader instanced_sprite_shader;
			size_t instanced_sprite_max_draws_per_call;
			Shader batch_sprite_shader;
			size_t batch_sprite_max_draws_per_call;
		};
	}

	extern ae::internal::DefaultAssetsType* DefaultAssets;
	extern ae::internal::AssetManagerType AssetManager;
}