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

////////////////////////////////////////////////////////////////////////////////////
/// Fonts
/// 
/// Storage:
///		All fonts are stored in a map of 
///		- a key value GlyphMetrics made of font size and bold strengths (vertical and horizontal),
///		- and mapped value being a FontSheet, which stores font's texture / bitmap of glyphs and also a map of loaded Glyphs.
///
/// Glyph Loading:
///		Each glyph is loaded whenever it is retrieved, except the 'missing glyph' - unicode's unknown character's glyph (0xFFFD),
///		which is also loaded when FontTexture is retrieved. All unretrievable glyphs will be return as defaultly constructed Glyphs.
/// 
/// FontTexture:
///		In order to save space, FontTexture was made a GL_RED - one channel only texture.
///		
///		Other OpenGL parameters:
///		- GL_TEXTURE_WRAP_S, GL_TEXTURE_WRAP_T = GL_REPEAT,
///		- GL_TEXTURE_MIN_FILTER = GL_LINEAR,
///		- GL_TEXTURE_MAG_FILTER = GL_NEAREST,
///		- mipmaps are not generated.
/// 
/// Glyphs:
///		Glyph class consists of 
///		- texture coordinates of it's bitmap, 
///		- advance being the offset to another character in text,
///		- bearing being the offset from baseline,
///		- size being the actual size of the glyph.
/// 
/// Clearing Sheets:
///		FontTextures are stored based on size and bold strengths, sometimes it is necessary to remove a few old bitmaps to clean up memory.
///
////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../System/Vector2.hpp"
#include "TextureCanvas.hpp"
#include "Texture.hpp"

#include <string>
#include <map>
#include <unordered_map>
#include <memory>

namespace ae {

	class Font;
	namespace internal { struct FontSheet; }

	namespace internal {
		bool InitializeFontLibrary();
		void TerminateFontLibrary();
	}

	struct Glyph {
		Vector2ui texcoords;
		unsigned int advance = 0;
		Vector2ui size;
		Vector2i bearing;
	};

	class FontTexture {
		friend class Font;
		friend struct internal::FontSheet;

	public:
		~FontTexture();

		void Bind(int sampler2d_slot = 0) const;
		void Unbind() const;

		const unsigned char* GetPixelData() const;
		const Vector2ui& GetSize() const;

	private:
		std::uint32_t m_texture_id;
		Vector2ui m_size;
		unsigned char* m_pixel_data;
		bool m_update;

		FontTexture(const Vector2ui& size);
		FontTexture(FontTexture&&) = delete;
		FontTexture(const FontTexture&) = delete;

		void Resize(const Vector2ui& new_size, unsigned char empty_color = 0);
		unsigned char& Px(unsigned int x, unsigned int y);
		void UpdateTexture();
	};


	namespace internal {
		struct GlyphMetrics {
			unsigned int size;
			Vector2uc bold;

			GlyphMetrics(unsigned int size, const Vector2uc& bold)
				: size(size), bold(bold) {}

			struct Hash {
				size_t operator()(const GlyphMetrics& metrics) const {
					return static_cast<size_t>(((metrics.size << 8) + metrics.bold.x) << 8 + metrics.bold.y);
				}
			};

			bool operator==(const GlyphMetrics& other) const {
				return (size == other.size && bold == other.bold);
			}
		};

		struct FontSheet {
			std::map<char32_t, Glyph> glyphs;
			unsigned int bitmap_pos;
			FontTexture bitmap;

			FontSheet();
			FontSheet(const FontSheet&) = default;
			FontSheet(FontSheet&&) = default;
		};

	}

	class Font {
	public:
		Font() = default;
		Font(Font&& move) noexcept;
		Font(const Font&) = delete;
		~Font();

		bool LoadFromFile(const std::string& filename);
		bool LoadFromData(const void* data, size_t size);

		bool WasLoaded() const;

		const Glyph& RetrieveGlyph(char32_t code, unsigned int size, const Vector2uc& bold_strength = Vector2uc(0, 0)) const;
		const FontTexture& RetrieveTexture(unsigned int size, const Vector2uc& bold_strength = Vector2uc(0, 0)) const;

		float GetLineSpacing(unsigned int size, unsigned char bold_y = 0) const;
		bool HasGlyph(char32_t code) const;

		float GetUnderlinePosition(unsigned char size) const;
		float GetLineThickness(unsigned char size) const;

		const std::string& GetFamily() const;

		// clear sheets
		void ClearSheets();
		void ClearSheet(unsigned int size, const Vector2uc& bold_strength = Vector2uc(0, 0));

	private:
		void* m_native_face = nullptr;
		std::string m_family;

		mutable std::unordered_map<internal::GlyphMetrics, std::unique_ptr<internal::FontSheet>, internal::GlyphMetrics::Hash> m_sheets;

		Vector2ui DrawToSheet(unsigned char* buffer, const Vector2ui& size, internal::FontSheet& sheet) const;
		void EmboldenGlyph(void* glyph_slot, const Vector2uc& strength) const;

		decltype(m_sheets)::iterator FindOrCreateSheet(const internal::GlyphMetrics& metrics) const;
		void LoadMissingGlyph(decltype(m_sheets)::iterator sheet_it) const;
	};

}