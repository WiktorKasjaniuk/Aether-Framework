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

#include "../System/Vector2.hpp"
#include "../System/Vector3.hpp"
#include "../System/Rectangle.hpp"

namespace ae {

	class Matrix3x3;

	Vector3f operator*(const Matrix3x3& left, const Vector3f& right);
	Matrix3x3 operator*(const Matrix3x3& left, const Matrix3x3& right);
	Matrix3x3& operator*=(Matrix3x3& left, const Matrix3x3& right);

	bool operator==(const Matrix3x3& left, const Matrix3x3& right);
	bool operator!=(const Matrix3x3& left, const Matrix3x3& right);

	class Matrix3x3 {
		friend ae::Vector3f ae::operator*(const Matrix3x3& left, const ae::Vector3f& right);
		friend Matrix3x3 ae::operator*(const Matrix3x3& left, const Matrix3x3& right);
		friend Matrix3x3& ae::operator*=(Matrix3x3& left, const Matrix3x3& right);
		friend bool ae::operator==(const Matrix3x3& left, const Matrix3x3& right);
		friend bool ae::operator!=(const Matrix3x3& left, const Matrix3x3& right);

	public:
		Matrix3x3() = default;
		Matrix3x3(const Matrix3x3& copy);
		Matrix3x3(const float copy[3][3]);
		Matrix3x3(
			float v00, float v10, float v20,
			float v01, float v11, float v21,
			float v02, float v12, float v22
		);

		Matrix3x3 GetTranspose() const;
		float GetDeterminant() const;
		Matrix3x3 GetInverse() const;

		Matrix3x3 GetTranslated(const Vector2f& translation) const;
		Matrix3x3 GetScaled(float factor) const;
		Matrix3x3 GetScaled(const Vector2f& factor) const;
		Matrix3x3 GetRotated(float rotation) const;
		Matrix3x3 GetSheared(const Vector2f& shear) const;

		Matrix3x3& Translate(const Vector2f& translation);
		Matrix3x3& Scale(float factor);
		Matrix3x3& Scale(const Vector2f& factor);
		Matrix3x3& Rotate(float rotation);
		Matrix3x3& Shear(const Vector2f& shear);

		Vector2f TransformPoint(const Vector2f& point) const;
		FloatRect TransformRectangle(const FloatRect& rectangle) const;

		Matrix3x3& operator=(const Matrix3x3& right);
		float* const operator[](size_t column);
		const float* const operator[](size_t column) const;
		float* GetArray() { return m_matrix[0]; }
		const float* GetArray() const { return m_matrix[0]; };

		static const Matrix3x3 Identity;

	private:
		float m_matrix[3][3];
	};
}