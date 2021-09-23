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


#include "Graphics/Font.hpp"
#include "Core/OpenGLCalls.hpp"
#include "Core/Preprocessor.hpp"
#include "System/LogError.hpp"

#include <glad/glad.h>

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_BITMAP_H
#include FT_STROKER_H

#define AE_FONT_LOAD_ASSERT(string_operation) \
if (!WasLoaded()) { \
	AE_ASSERT_FALSE("Could not " << string_operation << " from font, it has not been loaded yet"); \
	LogError("Could " string_operation " from font, it has not been loaded yet. Perhaps the file missing?", true); \
	std::exit(EXIT_FAILURE); \
}

FT_Library g_library;

// each glyph in bitmap must have additional space in order to not generate aritifacts due to antialiasing
constexpr unsigned int c_bitmap_spacing = 5;

namespace ae {

	namespace internal {
		bool InitializeFontLibrary() {
			return (FT_Init_FreeType(&g_library) == FT_Err_Ok);
		}
		void TerminateFontLibrary() {
			FT_Done_FreeType(g_library);
		}
	}

	const unsigned char* FontTexture::GetPixelData() const { return m_pixel_data; }
	const Vector2ui& FontTexture::GetSize() const { return m_size; }

	FontTexture::FontTexture(const Vector2ui& size)
		: m_pixel_data(NULL), m_update(false), m_size(size)
	{
		// generate texture
		AE_GL_LOG(glGenTextures(1, &m_texture_id));

		// bind
		Bind();

		// store
		AE_GL_LOG(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
		AE_GL_LOG(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, 0, 0, 0, GL_RED, GL_UNSIGNED_BYTE, NULL));

		// set parameters
		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		AE_GL_LOG(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	}
	FontTexture::~FontTexture() {
		AE_GL_LOG(glDeleteTextures(1, &m_texture_id));
		delete[] m_pixel_data;
	}

	void FontTexture::Resize(const Vector2ui& new_size, unsigned char empty_color) {

		unsigned char* old_pixels = m_pixel_data;
		m_pixel_data = new unsigned char[new_size.x * new_size.y];

		for (unsigned int x(0); x < new_size.x; x++)
			for (unsigned int y(0); y < new_size.y; y++) {

				// fill with empty_color
				if (x >= m_size.x || y >= m_size.y)
					m_pixel_data[new_size.x * y + x] = empty_color;

				// paste previous texture
				else
					m_pixel_data[new_size.x * y + x] = old_pixels[m_size.x * y + x];
			}


		delete[] old_pixels;
		m_size = new_size;
		m_update = true;
	}
	void FontTexture::UpdateTexture() {
		if (m_update) {

			// new storage
			Bind();
			AE_GL_LOG(glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, m_size.x, m_size.y, 0, GL_RED, GL_UNSIGNED_BYTE, m_pixel_data));

			m_update = false;
		}
	}

	unsigned char& FontTexture::Px(unsigned int x, unsigned int y) {
		return m_pixel_data[m_size.x * y + x];
	}

	void FontTexture::Bind(int sampler2d_slot) const {
		AE_GL_LOG(glActiveTexture(GL_TEXTURE0 + sampler2d_slot));
		AE_GL_LOG(glBindTexture(GL_TEXTURE_2D, m_texture_id));
	}
	void FontTexture::Unbind() const {
		AE_GL_LOG(glBindTexture(GL_TEXTURE_2D, 0));
	}

	Font::~Font() {
		FT_Done_Face(static_cast<FT_Face>(m_native_face));
	}
	Font::Font(Font&& move) noexcept {
		m_native_face = move.m_native_face;
		m_family = std::move(move.m_family);
		m_sheets = std::move(m_sheets);

		move.m_native_face = 0;
	}

	internal::FontSheet::FontSheet()
		: bitmap(Vector2ui(c_bitmap_spacing, 0)), bitmap_pos(c_bitmap_spacing) {}

	const std::string& Font::GetFamily() const { return m_family; }
	void Font::ClearSheets() {

		// assert loaded
		AE_FONT_LOAD_ASSERT("clear sheets");

		m_sheets.clear();
	}
	void Font::ClearSheet(unsigned int size, const Vector2uc& bold_strength) {

		// assert loaded
		AE_FONT_LOAD_ASSERT("clear sheet");

		// find & erase
		auto it = m_sheets.find(internal::GlyphMetrics(size, bold_strength));
		if (it != m_sheets.end())
			m_sheets.erase(it);
	}
	bool Font::LoadFromFile(const std::string& filename) {

		AE_ASSERT(!WasLoaded(), "Font has already been loaded");

		FT_Face face;
		
		// create new face
		if (FT_New_Face(g_library, filename.c_str(), 0, &face) != FT_Err_Ok)
			return false;

		if (face->family_name)
			m_family = face->family_name;

		m_native_face = face;
		return true;
	}
	bool Font::LoadFromData(const void* data, size_t size) {

		AE_ASSERT(!WasLoaded(), "Font has already been loaded");

		FT_Face face;

		// create new face
		if (FT_New_Memory_Face(g_library, static_cast<const FT_Byte*>(data), size, 0, &face) != FT_Err_Ok)
			return false;

		if (face->family_name)
			m_family = face->family_name;

		m_native_face = face;
		return true;

	}
	bool Font::WasLoaded() const { return (m_native_face != nullptr); }

	decltype(Font::m_sheets)::iterator Font::FindOrCreateSheet(const internal::GlyphMetrics& metrics) const {

		// find
		auto sheet_it = m_sheets.find(metrics);

		// create
		if (sheet_it == m_sheets.end()) {
			sheet_it = m_sheets.insert(std::make_pair(metrics, new internal::FontSheet())).first;

			LoadMissingGlyph(sheet_it);
		}

		// return
		return sheet_it;
	}

	void Font::LoadMissingGlyph(decltype(m_sheets)::iterator sheet_it) const {
		
		constexpr char32_t missing_char = 0xFFFD;
		auto& glyphs = sheet_it->second->glyphs;
		const internal::GlyphMetrics& metrics = sheet_it->first;

		// load glyph
		FT_Face face = static_cast<FT_Face>(m_native_face);

		// assert loaded
		AE_FONT_LOAD_ASSERT("load missing glyph");

		bool could_load_glyph =
			metrics.size != 0 &&
			FT_Set_Pixel_Sizes(face, 0, metrics.size) == FT_Err_Ok &&
			FT_Load_Char(face, missing_char, FT_LOAD_RENDER) == FT_Err_Ok
		;

		if (!could_load_glyph) {

			// no missing glyph in font, create empty one
			glyphs.insert(std::make_pair(missing_char, std::move(Glyph())));

			return;
		}
		
		auto& ft_glyph = face->glyph;

		// embolden if needed
		if (metrics.bold.x != 0 || metrics.bold.y != 0)
			EmboldenGlyph(ft_glyph, metrics.bold);

		// update sheet
		Vector2ui coords = DrawToSheet(ft_glyph->bitmap.buffer, Vector2ui(ft_glyph->bitmap.width, ft_glyph->bitmap.rows), *sheet_it->second);

		// create glyph
		Glyph glyph;
		glyph.texcoords = coords;
		glyph.advance = (ft_glyph->advance.x >> 6) + static_cast<unsigned int>(metrics.bold.x);
		glyph.bearing = Vector2i(ft_glyph->bitmap_left, ft_glyph->bitmap_top + static_cast<int>(metrics.bold.y));
		glyph.size = Vector2ui(ft_glyph->bitmap.width, ft_glyph->bitmap.rows);

		// missing glyph found, insert it
		glyphs.insert(std::make_pair(missing_char, std::move(glyph)));
	}

	float Font::GetLineSpacing(unsigned int size, unsigned char bold_y) const {
		FT_Face face = static_cast<FT_Face>(m_native_face);

		// assert loaded
		AE_FONT_LOAD_ASSERT("return line spacing");

		// no font
		if (FT_Set_Pixel_Sizes(face, 0, size) != FT_Err_Ok)
			return 0.f;

		// return
		return
			static_cast<float>(face->size->metrics.height)
			/ static_cast<float>(1 << 6) // convert to 26.6 format
			+ static_cast<float>(bold_y);
	}
	float Font::GetUnderlinePosition(unsigned char size) const {

		FT_Face face = static_cast<FT_Face>(m_native_face);

		// assert loaded
		AE_FONT_LOAD_ASSERT("return underline position");

		if (FT_Set_Pixel_Sizes(face, 0, size) == FT_Err_Ok) {

			if (FT_IS_SCALABLE(face) != FT_Err_Ok)
				return size / 10.f;

			return static_cast<float>(face->underline_position) / static_cast<float>(1 << 6);
		}

		return 0.f;

	}
	float Font::GetLineThickness(unsigned char size) const {

		FT_Face face = static_cast<FT_Face>(m_native_face);

		// assert loaded
		AE_FONT_LOAD_ASSERT("return line thickness");

		if (FT_Set_Pixel_Sizes(face, 0, size) == FT_Err_Ok) {

			if (FT_IS_SCALABLE(face) != FT_Err_Ok)
				return size / 14.f;

			return static_cast<float>(face->underline_thickness) / static_cast<float>(1 << 6);
		}

		return 0.f;
	}

	bool Font::HasGlyph(char32_t code) const {
		FT_Face face = static_cast<FT_Face>(m_native_face);

		// assert loaded
		AE_FONT_LOAD_ASSERT("return if has glyph");

		return (face && FT_Get_Char_Index(face, code) != 0);
	}
	void Font::EmboldenGlyph(void* glyph_slot, const Vector2uc& strength) const {

		// convert to 26.6 units (1/64)
		FT_Pos x = static_cast<FT_Pos>(strength.x) << 6;
		FT_Pos y = static_cast<FT_Pos>(strength.y) << 6;

		// embolden
		FT_GlyphSlot&& glyph = static_cast<FT_GlyphSlot>(glyph_slot);
		FT_Bitmap_Embolden(g_library, &glyph->bitmap, x, y);
	}

	const FontTexture& Font::RetrieveTexture(unsigned int size, const Vector2uc& bold_strength) const {
		// assert loaded
		AE_FONT_LOAD_ASSERT("retrieve texture");

		auto it = FindOrCreateSheet(internal::GlyphMetrics(size, bold_strength));
		it->second->bitmap.UpdateTexture();
		return it->second->bitmap;
	}

	const Glyph& Font::RetrieveGlyph(char32_t code, unsigned int size, const Vector2uc& bold_strength) const {
		FT_Face face = static_cast<FT_Face>(m_native_face);

		// assert loaded
		AE_FONT_LOAD_ASSERT("retrieve glyph");

		// find sheet
		auto sheet_it = FindOrCreateSheet(internal::GlyphMetrics(size, bold_strength));

		// find glyph
		auto& glyphs = sheet_it->second->glyphs;
		auto glyph_it = glyphs.find(code);

		if (glyph_it != glyphs.end())
			return glyph_it->second;

		// load glyph
		bool could_load_glyph =
			size != 0 &&
			FT_Set_Pixel_Sizes(face, 0, size) == FT_Err_Ok &&
			FT_Load_Char(face, code, FT_LOAD_RENDER) == FT_Err_Ok
		;

		if (!could_load_glyph) {
			glyphs.insert(std::make_pair(std::move(code), std::move(Glyph())));
			return glyphs.at(code);
		}

		auto& ft_glyph = face->glyph;

		// return missing glyph if does not exist
		if (!HasGlyph(code))
			return glyphs.at(0xFFFD);

		// embolden if needed
		if (bold_strength.x != 0 || bold_strength.y != 0)
			EmboldenGlyph(ft_glyph, bold_strength);

		// update sheet
		Vector2ui coords = DrawToSheet(ft_glyph->bitmap.buffer, Vector2ui(ft_glyph->bitmap.width, ft_glyph->bitmap.rows), *sheet_it->second);

		// create new glyph
		Glyph glyph;
		glyph.texcoords = coords;
		glyph.advance = (ft_glyph->advance.x >> 6) + static_cast<unsigned int>(bold_strength.x);
		glyph.bearing = Vector2i(ft_glyph->bitmap_left, ft_glyph->bitmap_top + static_cast<int>(bold_strength.y));
		glyph.size = Vector2ui(ft_glyph->bitmap.width, ft_glyph->bitmap.rows);

		glyphs.insert(std::make_pair(std::move(code), std::move(glyph)));

		// return
		return glyphs.at(code);
	}
	Vector2ui Font::DrawToSheet(unsigned char* buffer, const Vector2ui& size, internal::FontSheet& sheet) const {

		FontTexture& bitmap = sheet.bitmap;
		unsigned int& bitmap_pos = sheet.bitmap_pos;

		// update sheet size
		Vector2ui bitmap_size = bitmap.GetSize();
		bitmap_size.x += size.x + c_bitmap_spacing;

		if (bitmap_size.y < size.y + 2 * c_bitmap_spacing)
			bitmap_size.y = size.y + 2 * c_bitmap_spacing;

		bitmap.Resize(bitmap_size);

		// draw to sheet
		for (unsigned int x(0); x < size.x; x++)
			for (unsigned int y(0); y < size.y; y++)
				bitmap.Px(bitmap_pos + x, y + c_bitmap_spacing) = buffer[size.x * y + x];

		// move to new character position and return old one
		unsigned int old_bitmap_pos = bitmap_pos;
		bitmap_pos += size.x + c_bitmap_spacing;

		return Vector2ui(old_bitmap_pos, c_bitmap_spacing);
	}

}