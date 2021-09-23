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

#pragma once

#include "../System/Vector4.hpp"

namespace ae {

	struct Color {
		unsigned char r, g, b, a;

		Color();
		Color(const Color&) = default;
		Color(unsigned char r, unsigned char g, unsigned char b);
		Color(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		Color(const Vector4f& normalized);
		
		Vector4f GetNormalized() const;
		Color GetInverse() const;
		Color GetInverseAlpha() const;

		static const Color White;
		static const Color Black;
		static const Color Silver;
		static const Color Gold;
		static const Color Nova;
		static const Color Reddish;
		static const Color Cavern;
		static const Color Discord;
		static const Color Umati;
		static const Color Alpha;
		static const Color Matrix;
		static const Color Transparent;
	};

	// Multiplication
	Color operator*(const Color& left, const Color& right);
	Color operator*(const Color& left, const Vector4f& right);
	Color operator*(const Color& left, float right);

	Color& operator*=(Color& left, const Color& right);
	Color& operator*=(Color& left, const Vector4f& right);
	Color& operator*=(Color& left, float right);

	// Division
	Color operator/(const Color& left, float right);
	Color& operator/=(Color& left, float right);

	// Comparation
	bool operator==(const Color& left, const Color& right);
	bool operator!=(const Color& left, const Color& right);

}