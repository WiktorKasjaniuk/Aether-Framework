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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Text
///
/// Updates:
///		Text geometry updates only
///			when a parameter has changed 
///			and the geometry is needed fe. for drawing or retrieving bounds
///
/// Parameters:
///		> Bold strength can be chosen in both vertical and horizontal direction,
///		> Italic shear is always 12 degrees
///		> Spacing:
///			Character spacing and line spacing can be set by using a float factor,
///			to retrieve the total amount of pixels needed to advance to the new line use GetLineSpacingValue()
/// 
///	Coordinates and Bounds:
///		All glyphs are located in a 2D space, where
///		x axis is the baseline for the first line of glyphs,
///		y axis is the origin line for the first glyhps of all lines (that means origin points for all the first glyphs in all lines are located on y axis),
///			some glyphs that are sheared and below baseline can have negative x coordinates.
/// 
///		GetBounds() returns the smallest rectangle in which the text along with strikeline and underline can fit
/// 
/// Char Information:
/// CalculateCharMetrics functions return information about found characters in a form of a CharMetrics struct.
/// This includes 
///		> vertices positions'(top left, top right, bot right, bot left),
///		> glyph's bound (sheared glyphs have bigger bounds),
///		> found glyph's advance and the advance of his left neighbor,
///		> character index in string,
///		> and the character.
///
/// Used Shaders:
///		Vertex Shader for text: --------------------------------------------------------------------------------------------------------------
///			#version 460 core
///		
///			layout(location = 0) in vec2 a_position;
///			layout(location = 1) in vec2 a_texcoords;
///		
///			out vec2 v_texcoords;
///		
///			uniform mat3 u_mvp;
///		
///			void main()
///			{
///				gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0);
///				v_texcoords = a_texcoords;
///			}
/// 
///		Fragment Shader for text: --------------------------------------------------------------------------------------------------------------
///			#version 460 core
///		
///			layout(location = 0) out vec4 a_color;
///		
///			in vec4 v_color;
///			in vec2 v_texcoords;
///		
///			uniform sampler2D u_texture;
///			uniform vec4 u_color;
///		
///			void main()
///			{ 
///				// normalize pixel texture coords 
///				vec2 normalized_coords = v_texcoords / vec2(textureSize(u_texture, 0));
///				
///				// output color 
///				float alpha = texture(u_texture, normalized_coords).r;
///				a_color = vec4(u_color.rgb, u_color.a * alpha);
///			}
///			
///		Vertex Shader for lines: --------------------------------------------------------------------------------------------------------------
///			#version 460 core
///		
///			layout(location = 0) in vec2 a_position;
///		
///			uniform mat3 u_mvp;
///		
///			void main()
///			{
///				gl_Position = vec4( u_mvp * vec3(a_position, 1.0), 1.0);
///			}
///		
///		Fragment Shader for lines: --------------------------------------------------------------------------------------------------------------
///			#version 460 core
///		
///			layout(location = 0) out vec4 a_color;
///		
///			in vec4 v_color;
///		
///			uniform vec4 u_color;
///		
///			void main()
///			{ 
///				a_color = u_color;
///			}
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../Font.hpp"
#include "../Shader.hpp"
#include "VertexArray.hpp"
#include "../../System/Rectangle.hpp"
#include "../Matrix3x3.hpp"
#include "../../Structure/Camera.hpp"

#include <functional>

namespace ae {

	class Text {
	public:
		struct CharMetrics {
			Vector2f vertices[4]; // top left, top right, bot right, bot left
			FloatRect bounds;
			Vector2f last_advance, advance;
			char32_t character;
			size_t index;

			CharMetrics() : character(U'\0'), index(0) {}
			CharMetrics(
				const Vector2f& v0, const Vector2f& v1, const Vector2f& v2, const Vector2f& v3, 
				FloatRect bounds, const Vector2f& last_advance, const Vector2f& advance, char32_t character, size_t index)
				: bounds(bounds), last_advance(last_advance), advance(advance), character(character), index(index)
			{
				vertices[0] = v0;
				vertices[1] = v1;
				vertices[2] = v2;
				vertices[3] = v3;
			}
		};

		Text();
		Text(const Text&) = default;
		Text(Text&&) = default;

		Text& operator=(const Text&) = default;
		Text& operator=(Text&&) = default;
		
		void SetFont(const Font& font);
		void SetString(const std::u32string& string);
		void SetCharSize(unsigned int size);
		void SetColor(const Color& color);
		void SetBold(const Vector2uc& strength);
		void SetBold(unsigned char strength = 6);
		void SetItalicShear(bool shear);
		void SetLineSpacingFactor(float spacing);
		void SetCharSpacingFactor(float spacing);
		void SetUnderline(bool underline);
		void SetStrikeline(bool strikeline);

		const Font* GetFont() const;
		const std::u32string& GetString() const;
		unsigned int GetCharSize() const;
		const Color& GetColor() const;
		const Vector2uc& GetBold() const;
		float GetLineSpacingFactor() const;
		float GetCharSpacingFactor() const;
		bool IsItalicSheared() const;
		bool IsStrikeline() const;
		bool IsUnderline() const;

		float GetLineSpacingValue() const;
		const FloatRect& GetBounds() const;

		CharMetrics CalculateCharMetrics(size_t index) const;
		CharMetrics CalculateNearestCharMetrics(const Vector2f& point) const;

		void Draw(const Matrix3x3& transform = Camera.GetProjMatrix()) const;
		void Draw(const Shader& text_shader, const Shader& line_shader, const Matrix3x3& transform = Camera.GetProjMatrix()) const;
		void Draw(const std::function<void(const Font*, const std::u32string&, const Color&, bool underline, bool strikeline, const VertexArray<VertexPosTex>& text_vertices, const VertexArray<VertexPos>& line_vertices)>& draw) const;

		template <typename ...HandlerTypes>
		void Draw(const std::function<void(const Font*, const std::u32string&, const Color&, bool underline, bool strikeline, const VertexArray<VertexPosTex>& text_vertices, const VertexArray<VertexPos>& line_vertices, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const;

	private:
		const Font* m_font = nullptr;
		mutable bool m_update = false;

		// geometry
		mutable FloatRect m_bounds;
		mutable VertexArray<VertexPosTex> m_vertices;
		mutable VertexArray<VertexPos> m_line_vertices;

		// text parameters
		std::u32string m_string;
		unsigned int m_char_size = 0;
		Color m_color;
		Vector2uc m_bold;

		float 
			m_line_spacing_factor = 1.f,
			m_char_spacing_factor = 1.f;

		bool
			m_shear = false,
			m_underline = false, 
			m_strikeline = false;

		void Update() const;
	};

	// Previous template definitions
	template <typename ...HandlerTypes>
	void Text::Draw(const std::function<void(const Font*, const std::u32string&, const Color&, bool underline, bool strikeline, const VertexArray<VertexPosTex>& text_vertices, const VertexArray<VertexPos>& line_vertices, HandlerTypes& ...handlers)>& draw, HandlerTypes& ...handlers) const {
		Update();
		draw(m_font, m_string, m_color, m_underline, m_strikeline, m_vertices, m_line_vertices, handlers...);
	}

}