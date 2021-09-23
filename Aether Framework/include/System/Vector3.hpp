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
	struct Vector3 {

		Vector3<T>() : x(0), y(0), z(0) {}

		Vector3<T>(T x, T y, T z) : x(x), y(y), z(z) {}

		template <typename TCopy>
		Vector3<T>(const Vector3<TCopy>& copy) : x(static_cast<T>(copy.x)), y(static_cast<T>(copy.y)), z(static_cast<T>(copy.z)) {}

		union {
			T x, s, r;
		};
		union {
			T y, t, g;
		};
		union {
			T z, p, b;
		};
	};

	typedef Vector3<float>        Vector3f ;
	typedef Vector3<int>          Vector3i ;
	typedef Vector3<unsigned int> Vector3ui;
	typedef Vector3<bool>         Vector3b ;

	// Addition
	template <typename T>
	Vector3<T> operator+(const Vector3<T>& left, const Vector3<T>& right) { return Vector3<T>(left.x + right.x, left.y + right.y, left.z + right.z); }

	template <typename T>
	Vector3<T>& operator+=(Vector3<T>& left, const Vector3<T>& right) { left.x += right.x; left.y += right.y; left.z += right.z; return left; }

	// Subtraction and Negation
	template <typename T>
	Vector3<T> operator-(const Vector3<T>& left, const Vector3<T>& right) { return Vector3<T>(left.x - right.x, left.y - right.y, left.z - right.z); }

	template <typename T>
	Vector3<T> operator-(const Vector3<T>& left) { return Vector3<T>(-left.x, -left.y, -left.z); }

	template <typename T>
	Vector3<T>& operator-=(Vector3<T>& left, const Vector3<T>& right) { left.x -= right.x; left.y -= right.y; left.z -= right.z; return left; }

	// Multiplication
	template <typename T>
	Vector3<T> operator*(const Vector3<T>& left, const T& right) { return Vector3<T>(left.x * right, left.y * right, left.z * right); }

	template <typename T>
	Vector3<T>& operator*=(Vector3<T>& left, const T& right) { left.x *= right; left.y *= right; left.z *= right; return left; }

	// Division
	template <typename T>
	Vector3<T> operator/(const Vector3<T>& left, const T& right) { return Vector3<T>(left.x / right, left.y / right, left.z / right); }

	template <typename T>
	Vector3<T>& operator/=(Vector3<T>& left, const T& right) { left.x /= right; left.y /= right; left.z /= right; return left; }

	// Comparation
	template <typename T>
	bool operator==(const Vector3<T>& left, const Vector3<T>& right) { return (left.x == right.x && left.y == right.y && left.z == right.z); }

	template <typename T>
	bool operator!=(const Vector3<T>& left, const Vector3<T>& right) { return !(left.x == right.x && left.y == right.y && left.z == right.z); }

}


