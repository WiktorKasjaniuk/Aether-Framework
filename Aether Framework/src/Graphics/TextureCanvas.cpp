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


#include "Graphics/TextureCanvas.hpp"
#include "Structure/AssetManager.hpp"

#include "Core/Preprocessor.hpp"
#include "System/LogError.hpp"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image.h>
#include <stb_image/stb_image_write.h>

#include <algorithm>

#define AE_TEXTURE_CANVAS_SIZE_CHECK_COND(size_x, size_y) size_x != 0 && size_y != 0 && static_cast<uint64_t>(size_x) * size_y * 4 <= (size_t)-1

#define AE_TEXTURE_CANVAS_CHECK_RETURN(size_x, size_y, pixels) \
	\
	if ( !AE_TEXTURE_CANVAS_SIZE_CHECK_COND(size_x, size_y) ) { \
		\
		AE_WARNING("Could not create TextureCanvas, invalid size");\
		LogError("[Aether] Could not create TextureCanvas, invalid size", false); \
		\
		return false; \
	} 
	

namespace ae {

	unsigned int TextureCanvas::jpeg_quality = 100;
	TextureCanvas::~TextureCanvas() {
		if (m_free_pixel_data)
			delete[] m_pixels;
	}
	TextureCanvas::TextureCanvas(const TextureCanvas& copy) {
		*this = copy;
	}
	TextureCanvas::TextureCanvas(TextureCanvas&& move) noexcept {
		*this = std::move(move);
	}

	TextureCanvas& TextureCanvas::operator=(const TextureCanvas& copy) {
		Reset();

		m_free_pixel_data = copy.m_free_pixel_data;

		m_size = copy.m_size;

		m_pixels = new Color[m_size.x * m_size.y * 4];
		memcpy(m_pixels, copy.m_pixels, m_size.x * m_size.y * 4);

		return *this;
	}
	TextureCanvas& TextureCanvas::operator=(TextureCanvas&& move) noexcept {
		Reset();

		m_pixels = move.m_pixels;
		m_size = std::move(move.m_size);
		m_free_pixel_data = move.m_free_pixel_data;

		move.m_pixels = nullptr;
		move.m_free_pixel_data = false;

		return *this;
	}

	void TextureCanvas::SetDealocation(bool should_free_pixel_data) { m_free_pixel_data = should_free_pixel_data; }

	bool TextureCanvas::Create(const Vector2ui& size, const Color& fill_color) {

		if (m_pixels != nullptr)
			Reset();

		AE_TEXTURE_CANVAS_CHECK_RETURN(size.x, size.y, m_pixels);

		m_pixels = new Color[size.x * size.y]();

		if (fill_color != Color::White)
			for (size_t p(0); p < size.x * size.y; p++)
				m_pixels[p] = fill_color;
		
		m_size = size;
		return true;
	}
	bool TextureCanvas::Create(const Texture& texture) {

		if (m_pixels != nullptr)
			Reset();

		const Vector2ui& size = texture.GetSize();
		AE_TEXTURE_CANVAS_CHECK_RETURN(size.x, size.y, m_pixels);

		m_size = size;
		texture.CopyPixelData(m_pixels);

		return true;
	}
	bool TextureCanvas::Create(const std::string& filename) {

		if (m_pixels != nullptr)
			Reset();

		int width, height, channels;
		m_pixels = reinterpret_cast<Color*>(stbi_load(filename.c_str(), &width, &height, &channels, 4));

		AE_TEXTURE_CANVAS_CHECK_RETURN(width, height, m_pixels);

		if (!m_pixels) {
			
			AE_WARNING("Could not create TextureCanvas by loading '" << filename << '\'');
			ae::LogError("[Aether] Could not create TextureCanvas by loading '" + filename + '\'', false);
			return false;
		}
		
		m_size.x = width;
		m_size.y = height;

		return true;
	}
	bool TextureCanvas::Create(const Color* data, const Vector2ui& size) {

		if (m_pixels != nullptr)
			Reset();

		AE_TEXTURE_CANVAS_CHECK_RETURN(size.x, size.y, m_pixels);

		m_pixels = new Color[size.x * size.y]();
		memcpy(m_pixels, data, size.x * size.y * 4);
		m_size = size;

		return true;
	}
	bool TextureCanvas::Create(const unsigned char* data, const Vector2ui& size, unsigned char input_channels) {

		if (m_pixels != nullptr)
			Reset();

		AE_TEXTURE_CANVAS_CHECK_RETURN(size.x, size.y, m_pixels);
		AE_ASSERT(input_channels > 0 && input_channels <= 4, "Could not create TextureCanvas, invalid channel size");

		if (input_channels == 4)
			return Create(reinterpret_cast<const Color*>(data), size);

		m_pixels = new Color[size.x * size.y]();

		switch (input_channels)
		{
			case 3:
				for (size_t p(0); p < size.x * size.y; p++)
					m_pixels[p] = Color(
						static_cast<const unsigned char*>(data)[p * input_channels    ],
						static_cast<const unsigned char*>(data)[p * input_channels + 1],
						static_cast<const unsigned char*>(data)[p * input_channels + 2],
						255
					);
				break;

			case 2:
				for (size_t p(0); p < size.x * size.y; p++)
					m_pixels[p] = Color(
						static_cast<const unsigned char*>(data)[p * input_channels],
						0,
						0,
						static_cast<const unsigned char*>(data)[p * input_channels + 1]
					);
				break;

			case 1:
				for (size_t p(0); p < size.x * size.y; p++)
					m_pixels[p] = Color(
						255,
						255,
						255,
						static_cast<const unsigned char*>(data)[p * input_channels]
					);
				break;
		}

		m_size = size;

		return true;
		
	}
	bool TextureCanvas::CreateShare(Color* data, const Vector2ui& size) {

		if (m_pixels != nullptr)
			Reset();

		AE_TEXTURE_CANVAS_CHECK_RETURN(size.x, size.y, m_pixels);

		m_pixels = data;
		m_size = size;

		return true;
	}

	void TextureCanvas::Reset() {
		if (m_free_pixel_data)
			delete[] m_pixels;

		m_pixels = nullptr;
		m_size = Vector2ui();
		m_free_pixel_data = true;
	}

	const Color* TextureCanvas::GetPixelData() const {
		return m_pixels;
	}
	Color* TextureCanvas::GetRawPixelData() {
		return m_pixels;
	}
	const Vector2ui& TextureCanvas::GetSize() const {
		return m_size;
	}

	Color& TextureCanvas::Px(unsigned int x, unsigned int y) {
		return m_pixels[m_size.x * y + x];
	}
	Color& TextureCanvas::operator[](size_t index) {
		return m_pixels[index];
	}

	bool TextureCanvas::SaveToFile(const std::string& filename) const {

		AE_ASSERT(m_pixels, "Could not save TextureCanvas, it has not been created yet");

		size_t dot = filename.rfind('.');
		if (dot == std::string::npos || filename.length() < dot+2) {
			AE_WARNING("Could not save TextureCanvas to file '" << filename << "', extension not found");
			LogError("Could not save TextureCanvas to file '" + filename + "', extension not found", false);
			return false;
		}

		std::string extension = filename.substr( dot + 1, filename.length() );
		std::transform(extension.begin(), extension.end(), extension.begin(), std::tolower);
		
		bool success = false;

		if (extension == "png")
			success = stbi_write_png(filename.c_str(), m_size.x, m_size.y, 4, m_pixels, m_size.x * 4);

		else if (extension == "jpg" || extension == "jpeg")
			success = stbi_write_jpg(filename.c_str(), m_size.x, m_size.y, 4, m_pixels, jpeg_quality);

		else if (extension == "bmp")
			success = stbi_write_bmp(filename.c_str(), m_size.x, m_size.y, 4, m_pixels);

		else if (extension == "tga")
			success = stbi_write_tga(filename.c_str(), m_size.x, m_size.y, 4, m_pixels);

		else {
			AE_WARNING("Could not save TextureCanvas to file '" << filename << "', unknown extension");
			LogError("Could not save TextureCanvas to file '" + filename + "', unknown extension", false);
			return false;
		}

		if (!success) {
			AE_WARNING("Could not save TextureCanvas to file '" << filename << "', " << stbi_failure_reason());
			LogError("Could not save TextureCanvas to file '" + filename + "', " + stbi_failure_reason(), false);
			return false;
		}

		return true;
	}


	void TextureCanvas::Fill(const Color& color) {
		AE_ASSERT(m_pixels, "TextureCanvas must be created before any operations");

		for (size_t p(0); p < m_size.x * m_size.y; p++)
			m_pixels[p] = color;
	}

	void TextureCanvas::Crop(const Vector2ui& left_top, const Vector2ui& new_size) {

		AE_ASSERT(m_pixels, "TextureCanvas must be created before any operations");

		AE_ASSERT(
			(left_top.x + new_size.x <= m_size.x) &&
			(left_top.y + new_size.y <= m_size.y),
			"Could not cut TextureCanvas, invalid size"
		);

		Color* old_pixels = m_pixels;
		m_pixels = new Color[new_size.x * new_size.y];

		for (size_t x(0); x < new_size.x; x++) 
			for (size_t y(0); y < new_size.y; y++)
				m_pixels[new_size.x * y + x] = old_pixels[m_size.x * (y + left_top.y) + (x + left_top.x)];
		
		m_size = new_size;
		delete[] old_pixels;
	}
	void TextureCanvas::Paste(const TextureCanvas& copy, const Vector2i& place) {

		AE_ASSERT(m_pixels, "TextureCanvas must be created before any operations");
		
		if (place.x >= static_cast<int>(m_size.x) || place.y >= static_cast<int>(m_size.y)) 
			return;

		Vector2ui fixed_size(std::min(m_size.x - place.x, copy.m_size.x), std::min(m_size.y - place.y, copy.m_size.y));
		Vector2ui fixed_place(std::max(0, -place.x), std::max(0, -place.y));

		for (size_t x(fixed_place.x); x < fixed_size.x; x++)
			for (size_t y(fixed_place.y); y < fixed_size.y; y++)
				m_pixels[m_size.x * (y + place.y) + (x + place.x)] = copy.m_pixels[copy.m_size.x * y + x];
	}

	void TextureCanvas::Stretch(const Vector2ui& new_size) {

		AE_ASSERT(m_pixels, "TextureCanvas must be created before any operations");
		AE_ASSERT(AE_TEXTURE_CANVAS_SIZE_CHECK_COND(new_size.x, new_size.y), "Could not stretch TextureCanvas, invalid size");

		Color* old_pixels = m_pixels;
		m_pixels = new Color[new_size.x * new_size.y];

		for (size_t x(0); x < new_size.x; x++)
			for (size_t y(0); y < new_size.y; y++)
				m_pixels[new_size.x * y + x] = old_pixels[m_size.x * static_cast<size_t>(static_cast<float>(y) / new_size.y * m_size.y) + static_cast<size_t>(static_cast<float>(x) / new_size.x * m_size.x)];

		m_size = new_size;
		delete[] old_pixels;
	}

	void TextureCanvas::Resize(const Vector2ui& new_size, const Vector2i& left_top, const Color& empty_color) {

		AE_ASSERT(m_pixels, "TextureCanvas must be created before any operations");
		AE_ASSERT(AE_TEXTURE_CANVAS_SIZE_CHECK_COND(new_size.x, new_size.y), "Could not resize TextureCanvas, invalid size");

		Color* old_pixels = m_pixels;
		m_pixels = new Color[new_size.x * new_size.y];

		for (int x(0); x < new_size.x; x++)
			for (int y(0); y < new_size.y; y++) {

				// fill with empty_color
				if (x < left_top.x || x >= left_top.x + m_size.x || y < left_top.y || y >= left_top.y + m_size.y)
					m_pixels[new_size.x * y + x] = empty_color;

				// paste previous texture
				else
					m_pixels[new_size.x * y + x] = old_pixels[m_size.x * (y - left_top.y) + (x - left_top.x)];
			}


		m_size = new_size;
		delete[] old_pixels;
	}
}