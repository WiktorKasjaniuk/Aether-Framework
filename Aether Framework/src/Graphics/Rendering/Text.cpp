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

#include "Graphics/Rendering/Text.hpp"
#include "Structure/AssetManager.hpp"

#define AE_TEXT_SET_MEMBER(member, new_value) \
if (member != new_value) { \
	member = new_value; \
	m_update = true; \
}
#define AE_TEXT_SET_MEMBER_MIN(member, new_value, minimum) \
if (new_value < minimum) \
	new_value = minimum; \
 \
if (member != new_value) { \
	member = new_value; \
	m_update = true; \
}

#define AE_TEXT_SMALL_FLOAT -9999999999.f
#define AE_TEXT_LARGE_FLOAT  9999999999.f

// has to be between 0 and 90 degrees
constexpr float c_shear_degrees = 12.f;

static void AddLine(ae::VertexArray<ae::VertexPos>& vertex_array, float top, float width, float thickness) {

	std::vector<ae::VertexPos>&& vertices = {

		// top left
		ae::Vector2f(0.f, top),

		// top right
		ae::Vector2f(width, top),

		// bottom right
		ae::Vector2f(width, top + thickness),

		// bottom left
		ae::Vector2f(0.f, top + thickness),
	};

	unsigned int i = vertex_array.GetVertices().size();

	std::vector<unsigned int>&& indices = {
		i	 , i + 1, i + 2,
		i + 2, i + 3, i
	};

	vertex_array.Append(vertices, indices);
}

namespace ae {

	void Text::SetFont(const Font& font) { AE_TEXT_SET_MEMBER(m_font, &font); }
	void Text::SetString(const std::u32string& string) { AE_TEXT_SET_MEMBER(m_string, string); }
	void Text::SetCharSize(unsigned int size) { AE_TEXT_SET_MEMBER(m_char_size, size); }
	void Text::SetColor(const Color& color) { AE_TEXT_SET_MEMBER(m_color, color); }
	void Text::SetLineSpacingFactor(float factor) { AE_TEXT_SET_MEMBER_MIN(m_line_spacing_factor, factor, 0.f); }
	void Text::SetCharSpacingFactor(float factor) { AE_TEXT_SET_MEMBER_MIN(m_char_spacing_factor, factor, 0.f); }
	void Text::SetBold(const Vector2uc& strength) { AE_TEXT_SET_MEMBER(m_bold, strength); }
	void Text::SetBold(unsigned char strength) { AE_TEXT_SET_MEMBER(m_bold, Vector2uc(strength, strength)); }
	void Text::SetItalicShear(bool shear) { AE_TEXT_SET_MEMBER(m_shear, shear); }
	void Text::SetUnderline(bool underline) { AE_TEXT_SET_MEMBER(m_underline, underline); }
	void Text::SetStrikeline(bool strikeline) { AE_TEXT_SET_MEMBER(m_strikeline, strikeline); }

	const Font* Text::GetFont() const { return m_font; }
	const std::u32string& Text::GetString() const { return m_string; }
	unsigned int     Text::GetCharSize()          const { return m_char_size; }
	const Color& Text::GetColor()             const { return m_color; }
	const Vector2uc& Text::GetBold()              const { return m_bold; }
	float            Text::GetLineSpacingFactor() const { return m_line_spacing_factor; }
	float            Text::GetCharSpacingFactor() const { return m_char_spacing_factor; }
	bool             Text::IsItalicSheared()      const { return m_shear; }
	bool             Text::IsStrikeline()         const { return m_strikeline; }
	bool             Text::IsUnderline()          const { return m_underline; }

	float Text::GetLineSpacingValue() const {
		if (!m_font || !m_font->WasLoaded())
			return 0.f;
		return m_font->GetLineSpacing(m_char_size) * m_line_spacing_factor;
	}

	const FloatRect& Text::GetBounds() const {
		m_update = true;
		Update();
		return m_bounds;
	}

	Text::Text() {
		m_vertices.Bind();
		m_vertices.AddLayout<Vector2f>(0, offsetof(VertexPosTex, position), false);
		m_vertices.AddLayout<Vector2f>(1, offsetof(VertexPosTex, texcoords), false);

		m_line_vertices.Bind();
		m_line_vertices.AddLayout<Vector2f>(0, offsetof(VertexPos, position), false);
	}

	void Text::Draw(const Matrix3x3& transform) const {
		Draw(DefaultAssets->text_shader, DefaultAssets->color_shader, transform);
	}
	void Text::Draw(const Shader& text_shader, const Shader& line_shader, const Matrix3x3& transform) const {
		Update();

		if (!m_font || m_string.empty())
			return;

		// draw text
		text_shader.Bind();

		m_font->RetrieveTexture(m_char_size, m_bold).Bind(0);
		text_shader.SetUniform.Sampler2D("u_texture", 0);

		text_shader.SetUniform.Vec4f("u_color", m_color.GetNormalized());
		text_shader.SetUniform.Mat3x3("u_mvp", transform.GetArray());

		m_vertices.Bind();
		m_vertices.Draw();

		// draw lines
		if (m_strikeline || m_underline) {
			line_shader.Bind();

			line_shader.SetUniform.Vec4f("u_color", m_color.GetNormalized());
			line_shader.SetUniform.Mat3x3("u_mvp", transform.GetArray());

			m_line_vertices.Bind();
			m_line_vertices.Draw();
		}
	}
	void Text::Draw(const std::function<void(const Font*, const std::u32string&, const Color&, bool underline, bool strikeline, const VertexArray<VertexPosTex>& text_vertices, const VertexArray<VertexPos>& line_vertices)>& draw) const {
		Update();
		draw(m_font, m_string, m_color, m_underline, m_strikeline, m_vertices, m_line_vertices);
	}

	void Text::Update() const {

		// check if needs update
		if (!m_update)
			return;

		// updated
		m_update = false;

		// no font - no glyphs
		if (!m_font || !m_font->WasLoaded())
			return;

		// clear geometry
		m_vertices.Clear();
		m_line_vertices.Clear();
		m_bounds = FloatRect();

		// dont draw empty strings
		if (m_string.empty())
			return;

		// set visble information
		Vector2f next_char_pos;
		float whitespace_width = m_font->RetrieveGlyph(U' ', m_char_size, m_bold).advance * m_char_spacing_factor;
		float line_spacing = m_font->GetLineSpacing(m_char_size, m_bold.y) * m_line_spacing_factor;
		float shear = tan(c_shear_degrees / 180.f * 3.141592654f);

		float line_thickness = m_font->GetLineThickness(m_char_size);
		float underline_offset = m_font->GetUnderlinePosition(m_char_size);
		float strikeline_offset = m_char_size / -3.f;

		// for metrics calculation
		bool first_line = true;

		float border_top = AE_TEXT_LARGE_FLOAT; // -999999.f allows negative offsets
		float border_bot = AE_TEXT_SMALL_FLOAT;
		float border_right = AE_TEXT_SMALL_FLOAT;
		float border_left = AE_TEXT_LARGE_FLOAT;

		// right border for strikeline & underline
		float border_line_right = 0.f;

		// iterate over characters
		for (char32_t character : m_string) {

			// handle special characters
			switch (character)
			{
			case U' ': case U'\t':

				// update left border
				if (border_left > next_char_pos.x)
					border_left = next_char_pos.x;

				//                                            U' '                 U'\t'
				next_char_pos.x += (character == U' ') ? whitespace_width : whitespace_width * 4.f;
				
				// right border for strikeline & underline
				border_line_right = next_char_pos.x;

				// update right border
				if (border_right < next_char_pos.x)
					border_right = next_char_pos.x;

				continue;

			case U'\n':

				// add lines
				if (m_strikeline || m_underline) {

					if (m_strikeline) {
						AddLine(m_line_vertices, next_char_pos.y + strikeline_offset, border_line_right, line_thickness);
						border_top = std::min(border_top, next_char_pos.y + strikeline_offset);
						border_bot = std::max(border_bot, next_char_pos.y + strikeline_offset + line_thickness);
					}

					if (m_underline) {
						AddLine(m_line_vertices, next_char_pos.y + underline_offset, border_line_right, line_thickness);
						border_top = std::min(border_top, next_char_pos.y + underline_offset);
						border_bot = std::max(border_bot, next_char_pos.y + underline_offset + line_thickness);
					}

					border_left = std::min(border_left, 0.f);
					border_right = std::max(border_right, border_line_right);
				}

				next_char_pos.y += line_spacing;
				next_char_pos.x = 0.f;

				// update bottom border
				border_bot = std::max(border_bot, next_char_pos.y);

				first_line = false;

				continue;

			case U'\0': case U'\r':
				continue;

			default:
				break;
			}

			// create glyph verices
			const Glyph& glyph = m_font->RetrieveGlyph(character, m_char_size, m_bold);

			Vector2f size = glyph.size;
			Vector2f coords = glyph.texcoords;
			Vector2f bear = glyph.bearing;

			float xpos = next_char_pos.x + bear.x;
			float ypos = next_char_pos.y - bear.y;

			// italic shear above and below baseline
			float above_sh = 0.f;
			float below_sh = 0.f;

			if (m_shear) {
				above_sh = bear.y * shear;
				below_sh = (bear.y - size.y) * shear;
			}

			std::vector<VertexPosTex> new_vertices = {

				// top left
				VertexPosTex(Vector2f(xpos + above_sh, ypos), coords),

				// top right
				VertexPosTex(Vector2f(xpos + above_sh + size.x, ypos), Vector2f(coords.x + size.x, coords.y)),

				// bottom right
				VertexPosTex(Vector2f(xpos + below_sh + size.x, ypos + size.y), Vector2f(coords.x + size.x, coords.y + size.y)),

				// bottom left
				VertexPosTex(Vector2f(xpos + below_sh, ypos + size.y), Vector2f(coords.x, coords.y + size.y))
			};

			// right border for strikeline & underline
			border_line_right = new_vertices[1].position.x;

			// index vertices
			unsigned int i = m_vertices.GetVertices().size();
			std::vector<unsigned int> new_indices = {
				i	 , i + 1, i + 2,
				i + 2, i + 3, i
			};
			m_vertices.Append(new_vertices, new_indices);

			// update borders
			border_right = std::max(border_right, next_char_pos.x + bear.x + size.x + above_sh);
			border_left = std::min(border_left, xpos + below_sh);

			if (first_line && border_top > -bear.y)
				border_top = -bear.y;

			border_bot = std::max(border_bot, next_char_pos.y + size.y - bear.y);

			// advance to next character
			next_char_pos.x += static_cast<float>(glyph.advance) * m_char_spacing_factor;
		}

		// add lines
		if (m_strikeline || m_underline) {

			if (m_strikeline) {
				AddLine(m_line_vertices, next_char_pos.y + strikeline_offset, border_line_right, line_thickness);
				border_top = std::min(border_top, next_char_pos.y + strikeline_offset);
				border_bot = std::max(border_bot, next_char_pos.y + strikeline_offset + line_thickness);
			}

			if (m_underline) {
				AddLine(m_line_vertices, next_char_pos.y + underline_offset, border_line_right, line_thickness);
				border_top = std::min(border_top, next_char_pos.y + underline_offset);
				border_bot = std::max(border_bot, next_char_pos.y + underline_offset + line_thickness);
			}

			border_left = std::min(border_left, 0.f);
			border_right = std::max(border_right, border_line_right);
		}

		// save geometry
		if (border_top == AE_TEXT_LARGE_FLOAT) border_top = 0.f;
		if (border_bot == AE_TEXT_SMALL_FLOAT) border_bot = 0.f;
		if (border_right == AE_TEXT_SMALL_FLOAT) border_right = 0.f;
		if (border_left == AE_TEXT_LARGE_FLOAT) border_left = 0.f;

		m_bounds.top = border_top;
		m_bounds.left = border_left;
		m_bounds.height = border_bot - border_top;
		m_bounds.width = border_right - border_left;
	}

	Text::CharMetrics Text::CalculateCharMetrics(size_t index) const {

		// no font or index
		if (!m_font || !m_font->WasLoaded() || m_string.size() <= index)
			return CharMetrics();

		// unknown chars
		char32_t find_char = m_string[index];

		if (find_char == U'\r' ||
			find_char == U'\0')
			return CharMetrics();

		// set visble information
		Vector2f next_char_pos;
		float whitespace_width = m_font->RetrieveGlyph(U' ', m_char_size, m_bold).advance * m_char_spacing_factor;
		float line_spacing = m_font->GetLineSpacing(m_char_size, m_bold.y) * m_line_spacing_factor;
		float shear = tan(c_shear_degrees / 180.f * 3.141592654f);

		// iterate over characters
		for (size_t i(0); i < m_string.size(); i++) {
			char32_t character = m_string[i];

			// handle special characters
			switch (character)
			{
			case U' ': case U'\t':
			{
				float distance = (character == U' ') ? whitespace_width : whitespace_width * 4.f;

				// return if found
				if (i == index) {
					Vector2f
						left(next_char_pos),
						right(next_char_pos.x + distance, next_char_pos.y);

					FloatRect bounds(next_char_pos, Vector2f(distance, 0.f));

					Vector2f& last_advance = next_char_pos;
					Vector2f current_advance(last_advance.x + distance, last_advance.y);

					return CharMetrics(
						left, right, right, left,
						bounds,
						last_advance, current_advance,
						character,
						index
					);
				}

				// advance either
				next_char_pos.x += distance;

				continue;
			}

			case U'\n':
			{
				Vector2f& last_advance = next_char_pos;
				Vector2f current_advance(0.f, next_char_pos.y + line_spacing);

				// return if found
				if (i == index) {
					return CharMetrics(
						Vector2f(), Vector2f(), Vector2f(), Vector2f(),
						FloatRect(),
						last_advance, current_advance,
						character,
						index
					);
				}

				// advance either
				next_char_pos.y += line_spacing;
				next_char_pos.x = 0.f;

				continue;
			}

			case U'\0': case U'\r':
				continue;

			default:
				break;
			}

			// create glyph verices
			const Glyph& glyph = m_font->RetrieveGlyph(character, m_char_size, m_bold);

			// char found
			if (i == index) {

				Vector2f size = glyph.size;
				Vector2f bear = glyph.bearing;

				float xpos = next_char_pos.x + bear.x;
				float ypos = next_char_pos.y - bear.y;

				// italic shear above and below baseline
				float above_sh = 0.f;
				float below_sh = 0.f;

				if (m_shear) {
					above_sh = bear.y * shear;
					below_sh = (bear.y - size.y) * shear;
				}

				// return metrics
				Vector2f
					v0(xpos + above_sh, ypos),
					v1(xpos + above_sh + size.x, ypos),
					v2(xpos + below_sh + size.x, ypos + size.y),
					v3(xpos + below_sh, ypos + size.y);

				FloatRect bounds(v3.x, v0.y, above_sh + size.x - below_sh, size.y);

				Vector2f& last_advance = next_char_pos;
				Vector2f current_advance(last_advance.x + static_cast<float>(glyph.advance) * m_char_spacing_factor, last_advance.y);

				return CharMetrics(
					v0, v1, v2, v3,
					bounds,
					last_advance, current_advance,
					character, index
				);
			}

			// advance to next character
			next_char_pos.x += static_cast<float>(glyph.advance) * m_char_spacing_factor;
		}

		// error
		return CharMetrics();
	}

	Text::CharMetrics Text::CalculateNearestCharMetrics(const Vector2f& point) const {

		// no font or string
		if (!m_font || !m_font->WasLoaded() || m_string.empty())
			return CharMetrics();

		// set visble information
		Vector2f next_char_pos;
		float whitespace_width = m_font->RetrieveGlyph(U' ', m_char_size, m_bold).advance * m_char_spacing_factor;
		float line_spacing = m_font->GetLineSpacing(m_char_size, m_bold.y) * m_line_spacing_factor;
		float shear = tan(c_shear_degrees / 180.f * 3.141592654f);

		// calculate line
		unsigned int line_count = std::count(m_string.begin(), m_string.end(), U'\n') + 1;
		size_t line = static_cast<size_t>(std::max(0.f, point.y / line_spacing + 1.f));
		if (line >= line_count)
			line = line_count - 1;

		next_char_pos.y = line_spacing * line;

		// shorten string to that one line only
		size_t first = 0, last = m_string.size() - 1;
		{
			unsigned int cur_line = 0;
			bool line_found = false, new_line = true;

			for (size_t i(0); i < m_string.size(); i++) {

				// line found
				if (new_line && cur_line == line) {
					first = i;
					line_found = true;
				}

				new_line = false;

				// next line found
				if (m_string[i] == U'\n') {
					if (line_found) {
						last = i;
						break;
					}

					cur_line++;
					new_line = true;
				}
			}
		}
		
		// iterate over characters
		for (size_t i(first); i <= last; i++) {
			char32_t character = m_string[i];
			
			// handle special characters
			switch (character)
			{
			case U' ': case U'\t':
			{
				float distance = (character == U' ') ? whitespace_width : whitespace_width * 4.f;

				// char found
				if (point.x < next_char_pos.x + distance) {
					Vector2f
						left(next_char_pos),
						right(next_char_pos.x + distance, next_char_pos.y);

					FloatRect bounds(next_char_pos, Vector2f(distance, 0.f));

					Vector2f& last_advance = next_char_pos;
					Vector2f current_advance(last_advance.x + distance, last_advance.y);

					return CharMetrics(
						left, right, right, left,
						bounds,
						last_advance, current_advance,
						character,
						i
					);
				}

				// advance either
				next_char_pos.x += distance;

				continue;
			}

			case U'\n':
			{
				// end of line, return
				Vector2f& last_advance = next_char_pos;
				Vector2f current_advance(0.f, next_char_pos.y + line_spacing);

				return CharMetrics(
					Vector2f(), Vector2f(), Vector2f(), Vector2f(),
					FloatRect(),
					last_advance, current_advance,
					character,
					i
				);

				continue;
			}

			case U'\0': case U'\r':
				continue;

			default:
				break;
			}
			
			// create glyph verices
			const Glyph& glyph = m_font->RetrieveGlyph(character, m_char_size, m_bold);

			Vector2f& last_advance = next_char_pos;
			Vector2f current_advance(last_advance.x + static_cast<float>(glyph.advance) * m_char_spacing_factor, last_advance.y);

			// char found
			if (point.x < current_advance.x || i == last) {

				Vector2f size = glyph.size;
				Vector2f bear = glyph.bearing;

				float xpos = next_char_pos.x + bear.x;
				float ypos = next_char_pos.y - bear.y;

				// italic shear above and below baseline
				float above_sh = 0.f;
				float below_sh = 0.f;

				if (m_shear) {
					above_sh = bear.y * shear;
					below_sh = (bear.y - size.y) * shear;
				}

				// return metrics
				Vector2f
					v0(xpos + above_sh, ypos),
					v1(xpos + above_sh + size.x, ypos),
					v2(xpos + below_sh + size.x, ypos + size.y),
					v3(xpos + below_sh, ypos + size.y);

				FloatRect bounds(v3.x, v0.y, above_sh + size.x - below_sh, size.y);

				return CharMetrics(
					v0, v1, v2, v3,
					bounds,
					last_advance, current_advance,
					character,
					i
				);
			}

			// advance to next character
			next_char_pos.x += static_cast<float>(glyph.advance) * m_char_spacing_factor;
		}

		// error
		return CharMetrics();

	}

} // namespace ae