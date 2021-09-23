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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Texture Canvas
///
///	Class for editing 2D textures, it stores pixels in a form of Color array (4 channels per pixel).
/// 
/// Construction:
///		Image can be constructed:
///			> by moving another TextureCanvas,
///			> by copying another another TextureCanvas (size and pixel data are copied, but SetDealocation(bool) is set to true).
/// 
/// Creation:
///		All Creation functions return false and log error if unable to create image.
///		If image was already created, Reset() is called.
/// 
///		Image can be created:
///			> new from size and fill color,
///			> new from file,
/// 
///			> copied from texture,
///			> copied from Color buffer,
///			> copied from byte buffer 
///				4 channels - RGBA - Color(R,G,B,A), 
///				3 channels - RGB  - Color(R,G,B,255),
///				2 channels - RA   - Color(R,0,0,A), 
///				1 channel  - A    - Color(255, 255, 255, A),
/// 
///			> shared from Color buffer (PLEASE remember to SetDealocation(false) in case of sharing resources!).
/// 
/// Destruction:
///		Image will be deconstructed whenever deconstructor is called,
///		or can be manually reset with Reset() function.
/// 
///		In both cases pixel data will be freed depending on
///		value set by SetDealocation(bool) function,
///		the initial value is true.
/// 
///		Each time Reset() is called, value set by SetDealocation(bool) function is reset to true,
///		thus if you don't want to free pixels, you need to call SetDealocation(bool) every time you Reset() image.
///
/// Retrieving Pixels:
///		Pixels can be retrieved by operator[] or Pixel(x,y) functions:
///		Color& pixel = canvas[width * y + x];
///		Color& pixel = Pixel(x,y);
/// 
/// Image Operations:
///		TextureCanvas come with some basic function for editing images:
///			> Fill(color),
///				Fills whole image with one color,
/// 
///			> Crop(left_top, new_size),
///				Crops image to a smaller size,
/// 
///			> Paste(copy_canvas, place),
///				Pastes image into the canvas, cuts the copy image if it's too big,
/// 
///			> Stretch(new_size),
///				Resizes and stretches the image without using anti-aliasing,
/// 
///			> Resize(new_size, left_top, empty_color),
///				Resizes image without stretching, fills with empty_color and pastes old image into new place (if it was too big, it get's cut).
/// 
/// Saving To File
///		Images can be saved to file,
///		the available formats are: png, jpg, jpeg, bmp, tga.
///		For jpg / jpeg quality, you can set TextureCanvas' static member 'jpeg_quality' to a value between 0 and 100.
/// 
///		Returns false and logs error if unable to save image.
/// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../System/Vector2.hpp"
#include "Color.hpp"
#include "Texture.hpp"

#include <string>

namespace ae {
	
	class TextureCanvas {
	public:
		static unsigned int jpeg_quality;

		TextureCanvas() = default;
		TextureCanvas(const TextureCanvas& copy);
		TextureCanvas(TextureCanvas&& move) noexcept;
		~TextureCanvas();

		TextureCanvas& operator=(const TextureCanvas& copy);
		TextureCanvas& operator=(TextureCanvas&& move) noexcept;

		void SetDealocation(bool should_free_pixel_data);
		void Reset();

		bool Create(const Vector2ui& size, const Color& fill_color = Color::White);
		bool Create(const Texture& texture);
		bool Create(const std::string& filename);
		bool Create(const Color* data, const Vector2ui& size);
		bool Create(const unsigned char* data, const Vector2ui& size, unsigned char input_channels);
		bool CreateShare(Color* data, const Vector2ui& size);

		const Color* GetPixelData() const;
		Color* GetRawPixelData();
		const Vector2ui& GetSize() const;

		Color& Px(unsigned int x, unsigned int y);
		Color& operator[](size_t index);

		bool SaveToFile(const std::string& filename) const;

		void Fill(const Color& color);
		void Crop(const Vector2ui& left_top, const Vector2ui& new_size);
		void Paste(const TextureCanvas& copy, const Vector2i& place);
		void Stretch(const Vector2ui& new_size);
		void Resize(const Vector2ui& new_size, const Vector2i& left_top, const Color& empty_color = Color::White);

	private:
		Color* m_pixels = nullptr;
		Vector2ui m_size;
		bool m_free_pixel_data = true;
	};
}

