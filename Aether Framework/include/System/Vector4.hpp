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
	struct Vector4 {

		Vector4<T>() : x(0), y(0), z(0), w(0) {}

		Vector4<T>(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

		template <typename TCopy>
		Vector4<T>(const Vector4<TCopy>& copy) : x(static_cast<T>(copy.x)), y(static_cast<T>(copy.y)), z(static_cast<T>(copy.z)), w(static_cast<T>(copy.w)) {}

		union {
			T x, s, r;
		};
		union {
			T y, t, g;
		};
		union {
			T z, p, b;
		};
		union {
			T w, q, a;
		};
	};

	typedef Vector4<float>        Vector4f ;
	typedef Vector4<int>          Vector4i ;
	typedef Vector4<unsigned int> Vector4ui;
	typedef Vector4<bool>         Vector4b ;

	// Addition
	template <typename T>
	Vector4<T> operator+(const Vector4<T>& left, const Vector4<T>& right) { return Vector4<T>(left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w); }

	template <typename T>
	Vector4<T>& operator+=(Vector4<T>& left, const Vector4<T>& right) { left.x += right.x; left.y += right.y; left.z += right.z; left.w += right.w; return left; }

	// Subtraction and Negation
	template <typename T>
	Vector4<T> operator-(const Vector4<T>& left, const Vector4<T>& right) { return Vector4<T>(left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w); }

	template <typename T>
	Vector4<T> operator-(const Vector4<T>& left) { return Vector4<T>(-left.x, -left.y, -left.z, -left.w); }

	template <typename T>
	Vector4<T>& operator-=(Vector4<T>& left, const Vector4<T>& right) { left.x -= right.x; left.y -= right.y; left.z -= right.z; left.w -= right.w; return left; }

	// Multiplication
	template <typename T>
	Vector4<T> operator*(const Vector4<T>& left, const T& right) { return Vector4<T>(left.x * right, left.y * right, left.z * right, left.w * right); }

	template <typename T>
	Vector4<T>& operator*=(Vector4<T>& left, const T& right) { left.x *= right; left.y *= right; left.z *= right; left.w *= right; return left; }

	// Division
	template <typename T>
	Vector4<T> operator/(const Vector4<T>& left, const T& right) { return Vector4<T>(left.x / right, left.y / right, left.z / right, left.w / right); }

	template <typename T>
	Vector4<T>& operator/=(Vector4<T>& left, const T& right) { left.x /= right; left.y /= right; left.z /= right; left.w /= right; return left; }

	// Comparation
	template <typename T>
	bool operator==(const Vector4<T>& left, const Vector4<T>& right) { return (left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w); }

	template <typename T>
	bool operator!=(const Vector4<T>& left, const Vector4<T>& right) { return !(left.x == right.x && left.y == right.y && left.z == right.z && left.w == right.w); }

}


