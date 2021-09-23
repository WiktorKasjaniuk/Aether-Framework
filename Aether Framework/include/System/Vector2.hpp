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

namespace ae {

	template <typename T>
	struct Vector2 {

		Vector2<T>() : x(0), y(0) {}

		Vector2<T>(T x, T y) : x(x), y(y) {}

		template <typename TCopy>
		Vector2<T>(const Vector2<TCopy>& copy) : x(static_cast<T>(copy.x)), y(static_cast<T>(copy.y)) {}

		union {
			T x, s;
		};
		union {
			T y, t;
		};
	};

	typedef Vector2<float>         Vector2f;
	typedef Vector2<int>           Vector2i ;
	typedef Vector2<unsigned int>  Vector2ui;
	typedef Vector2<unsigned char> Vector2uc;
	typedef Vector2<bool>          Vector2b ;

	// Addition
	template <typename T>
	Vector2<T> operator+(const Vector2<T>& left, const Vector2<T>& right) { return Vector2<T>(left.x + right.x, left.y + right.y); }

	template <typename T>
	Vector2<T>& operator+=(Vector2<T>& left, const Vector2<T>& right) { left.x += right.x; left.y += right.y; return left; }

	// Subtraction and Negation
	template <typename T>
	Vector2<T> operator-(const Vector2<T>& left, const Vector2<T>& right) { return Vector2<T>(left.x - right.x, left.y - right.y); }

	template <typename T>
	Vector2<T> operator-(const Vector2<T>& left) { return Vector2<T>(-left.x, -left.y); }

	template <typename T>
	Vector2<T>& operator-=(Vector2<T>& left, const Vector2<T>& right) { left.x -= right.x; left.y -= right.y; return left; }

	// Multiplication
	template <typename T>
	Vector2<T> operator*(const Vector2<T>& left, const T& right) { return Vector2<T>(left.x * right, left.y * right); }

	template <typename T>
	Vector2<T>& operator*=(Vector2<T>& left, const T& right) { left.x *= right; left.y *= right; return left; }

	// Division
	template <typename T>
	Vector2<T> operator/(const Vector2<T>& left, const T& right) { return Vector2<T>(left.x / right, left.y / right); }

	template <typename T>
	Vector2<T>& operator/=(Vector2<T>& left, const T& right) { left.x /= right; left.y /= right; return left; }

	// Comparation
	template <typename T>
	bool operator==(const Vector2<T>& left, const Vector2<T>& right) { return (left.x == right.x && left.y == right.y); }

	template <typename T>
	bool operator!=(const Vector2<T>& left, const Vector2<T>& right) { return !(left.x == right.x && left.y == right.y); }

}


