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


#include "Graphics/Color.hpp"

namespace ae {

	Color::Color() : r(255), g(255), b(255), a(255) {}
	Color::Color(unsigned char r, unsigned char g, unsigned char b)					 : r(r), g(g), b(b), a(255) {}
	Color::Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : r(r), g(g), b(b), a(a)   {}
	Color::Color(const Vector4f& normalized) 
		: r(static_cast<unsigned int>(normalized.x * 255.f)),
		  g(static_cast<unsigned int>(normalized.y * 255.f)),
		  b(static_cast<unsigned int>(normalized.z * 255.f)),
		  a(static_cast<unsigned int>(normalized.w * 255.f)) {}

	Vector4f Color::GetNormalized() const {
		return Vector4f (
			r/255.f, g/255.f, b/255.f, a/255.f
		);
	}
	Color Color::GetInverse() const {
		return Color (
			255 - r,
			255 - g,
			255 - b,
			      a
		);
	}
	Color Color::GetInverseAlpha() const {
		return Color (
			255 - r,
			255 - g,
			255 - b,
			255 - a
		);
	}

	const Color Color::White       (255, 255, 255);
	const Color Color::Black       (  0,   0,   0);
	const Color Color::Silver      (130, 140, 140);
	const Color Color::Gold        (220, 140,  40);
	const Color Color::Nova        ( 80, 160, 200);
	const Color Color::Reddish     (200,  60,  30);
	const Color Color::Cavern      (  0,  20,  40);
	const Color Color::Discord     (100, 130, 200);
	const Color Color::Umati       (160, 140, 140);
	const Color Color::Matrix      ( 70, 220,  90);
	const Color Color::Alpha       (255, 255, 255, 128);
	const Color Color::Transparent (0, 0, 0, 0);

	// Multiplication
	Color operator*(const Color& left, const Color& right) {
		return Color (
			left.r * (right.r / 255.f),
			left.g * (right.g / 255.f),
			left.b * (right.b / 255.f),
			left.a * (right.a / 255.f)
		);
	}
	Color operator*(const Color& left, const Vector4f& right) {
		return Color (
			left.r * right.r,
			left.g * right.g,
			left.b * right.b,
			left.a * right.a
		);
	}
	Color operator*(const Color& left, float right) {
		return Color (
			left.r * right,
			left.g * right,
			left.b * right,
			left.a * right
		);
	}

	Color& operator*=(Color& left, const Color& right) {
		left.r = left.r * (right.r / 255.f);
		left.g = left.g * (right.g / 255.f);
		left.b = left.b * (right.b / 255.f);
		left.a = left.a * (right.a / 255.f);

		return left;
	}
	Color& operator*=(Color& left, const Vector4f& right) {
		left.r = left.r * right.r;
		left.g = left.g * right.g;
		left.b = left.b * right.b;
		left.a = left.a * right.a;

		return left;
	}
	Color& operator*=(Color& left, float right) {
		left.r = left.r * right;
		left.g = left.g * right;
		left.b = left.b * right;
		left.a = left.a * right;

		return left;
	}

	// Division
	Color operator/(const Color& left, float right) {
		return Color(
			left.r / right,
			left.g / right,
			left.b / right,
			left.a / right
		);
	}
	Color& operator/=(Color& left, float right) {
		left.r = left.r / right;
		left.g = left.g / right;
		left.b = left.b / right;
		left.a = left.a / right;

		return left;
	}

	// Comparation
	bool operator==(const Color& left, const Color& right) {
		return (
			left.r == right.r &&
			left.g == right.g &&
			left.b == right.b &&
			left.a == right.a
		);
	}
	bool operator!=(const Color& left, const Color& right) {
		return !(
			left.r == right.r &&
			left.g == right.g &&
			left.b == right.b &&
			left.a == right.a
		);
	}
}