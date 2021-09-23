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


#include "Graphics/Matrix3x3.hpp"

#include <string.h>
#include <math.h>
#include <algorithm>

namespace ae {

	constexpr unsigned char size = 3;
	// formula: matrix[column][row]

	namespace {
		static void Multiply(float output[3][3], const float m[3][3], const float n[3][3]) {

			float new_matrix[3][3];

			// mat3x3's column
			for (unsigned char c(0); c < size; c++)
			
				// mat3x3's row
				for (unsigned char r(0); r < size; r++) {

					new_matrix[r][c] = 0;

					// single cell's calculation
					for (unsigned char i(0); i < size; i++)
						new_matrix[r][c] += m[i][c] * n[r][i];
				}

			// Safely, copy to the output
			memcpy(output, new_matrix, sizeof(new_matrix));
		}
	} // namespace

	Matrix3x3::Matrix3x3(const Matrix3x3& copy) {
		memcpy(m_matrix, copy.m_matrix, sizeof(m_matrix));
	}
	Matrix3x3::Matrix3x3(const float copy[3][3]) {
		memcpy(m_matrix, copy, sizeof(m_matrix));
	}
	Matrix3x3::Matrix3x3(
		float v00, float v10, float v20,
		float v01, float v11, float v21,
		float v02, float v12, float v22
	) {
		m_matrix[0][0] = v00; m_matrix[1][0] = v10; m_matrix[2][0] = v20;
		m_matrix[0][1] = v01; m_matrix[1][1] = v11;	m_matrix[2][1] = v21;
		m_matrix[0][2] = v02; m_matrix[1][2] = v12;	m_matrix[2][2] = v22;
	}

	Matrix3x3 Matrix3x3::GetTranspose() const {
		
		return Matrix3x3 {
			m_matrix[0][0], m_matrix[0][1], m_matrix[0][2],
			m_matrix[1][0], m_matrix[1][1], m_matrix[1][2],
			m_matrix[2][0], m_matrix[2][1], m_matrix[2][2]
		};
	}

	float Matrix3x3::GetDeterminant() const {

		return float {
			  m_matrix[0][0] * (m_matrix[1][1] * m_matrix[2][2] - m_matrix[2][1] * m_matrix[1][2])
			+ m_matrix[1][0] * (m_matrix[2][1] * m_matrix[0][2] - m_matrix[0][1] * m_matrix[2][2])
			+ m_matrix[2][0] * (m_matrix[0][1] * m_matrix[1][2] - m_matrix[1][1] * m_matrix[0][2])
		};
	}

	Matrix3x3 Matrix3x3::GetInverse() const {
		float det = GetDeterminant();

		if (det == 0.f)
			return Identity;

		float v00 = (m_matrix[1][1] * m_matrix[2][2] - m_matrix[2][1] * m_matrix[1][2]) / det;
		float v10 = (m_matrix[2][0] * m_matrix[1][2] - m_matrix[2][2] * m_matrix[1][0]) / det;
		float v20 = (m_matrix[2][1] * m_matrix[1][0] - m_matrix[2][0] * m_matrix[1][1]) / det;

		float v01 = (m_matrix[2][1] * m_matrix[0][2] - m_matrix[0][1] * m_matrix[2][2]) / det;
		float v11 = (m_matrix[0][0] * m_matrix[2][2] - m_matrix[2][0] * m_matrix[0][2]) / det;
		float v21 = (m_matrix[2][0] * m_matrix[0][1] - m_matrix[0][0] * m_matrix[2][1]) / det;

		float v02 = (m_matrix[0][1] * m_matrix[1][2] - m_matrix[1][1] * m_matrix[0][2]) / det;
		float v12 = (m_matrix[0][2] * m_matrix[1][0] - m_matrix[0][0] * m_matrix[1][2]) / det;
		float v22 = (m_matrix[0][0] * m_matrix[1][1] - m_matrix[0][1] * m_matrix[1][0]) / det;
		
		return Matrix3x3 {
			v00, v10, v20,
			v01, v11, v21,
			v02, v12, v22
		};

	}

	Matrix3x3 Matrix3x3::GetTranslated(const Vector2f& translation) const {
		return
			*this *
			Matrix3x3 {
				1.f, 0.f, translation.x,
				0.f, 1.f, translation.y,
				0.f, 0.f, 1.f
			};
	}
	Matrix3x3 Matrix3x3::GetScaled(float factor) const {
		return GetScaled(Vector2f(factor, factor));
	};

	Matrix3x3 Matrix3x3::GetScaled(const Vector2f& factor) const {
		return
			*this *
			Matrix3x3 {
				factor.x, 0.f,      0.f,
				0.f,      factor.y, 0.f,
				0.f,      0.f,      1.f
		};
	}
	Matrix3x3 Matrix3x3::GetRotated(float rotation) const {

		float radians = rotation * 3.141592654f / 180.f;
		float sine = static_cast<float>(sin(radians));
		float cosine = static_cast<float>(cos(radians));

		return
			*this *
			Matrix3x3 {
				cosine, -sine,   0.f,
				sine,    cosine, 0.f,
				0.f,     0.f,    1.f
		};
	}
	Matrix3x3 Matrix3x3::GetSheared(const Vector2f& shear) const {
		return
			*this *
			Matrix3x3{
				1.f,     shear.x, 0.f,
				shear.y, 1.f,     0.f,
				0.f,     0.f,     1.f
		};
	}

	Matrix3x3& Matrix3x3::Translate(const Vector2f& translation) { *this = GetTranslated(translation); return *this; }
	Matrix3x3& Matrix3x3::Scale(float factor)					 { *this = GetScaled(factor);          return *this; }
	Matrix3x3& Matrix3x3::Scale(const Vector2f& factor)			 { *this = GetScaled(factor);          return *this; }
	Matrix3x3& Matrix3x3::Rotate(float rotation)				 { *this = GetRotated(rotation);       return *this; }
	Matrix3x3& Matrix3x3::Shear(const Vector2f& shear)			 { *this = GetSheared(shear);          return *this; }

	Vector2f Matrix3x3::TransformPoint(const Vector2f& point) const {
		return Vector2f(
			m_matrix[0][0] * point.x +
			m_matrix[1][0] * point.y +
			m_matrix[2][0],
			
			m_matrix[0][1] * point.x +
			m_matrix[1][1] * point.y +
			m_matrix[2][1]
		);
	}

	FloatRect Matrix3x3::TransformRectangle(const FloatRect& rectangle) const {

		// transform old rectangle
		Vector2f left_top     = TransformPoint(Vector2f(rectangle.left,                   rectangle.top));
		Vector2f right_top    = TransformPoint(Vector2f(rectangle.left + rectangle.width, rectangle.top));
		Vector2f right_bottom = TransformPoint(Vector2f(rectangle.left + rectangle.width, rectangle.top + rectangle.height));
		Vector2f left_bottom  = TransformPoint(Vector2f(rectangle.left,                   rectangle.top + rectangle.height));

		// create new, bounding rectangle
		float left   = std::min(std::min(left_top.x, right_top.x), std::min(right_bottom.x, left_bottom.x));
		float right  = std::max(std::max(left_top.x, right_top.x), std::max(right_bottom.x, left_bottom.x));
		float top    = std::min(std::min(left_top.y, right_top.y), std::min(right_bottom.y, left_bottom.y));
		float bottom = std::max(std::max(left_top.y, right_top.y), std::max(right_bottom.y, left_bottom.y));

		return Rectangle(left, top, right - left, bottom - top);
	}

	const Matrix3x3 Matrix3x3::Identity {
		1.f, 0.f, 0.f,
		0.f, 1.f, 0.f,
		0.f, 0.f, 1.f 
	};

	Matrix3x3& Matrix3x3::operator=(const Matrix3x3& right) {
		memcpy(m_matrix, right.m_matrix, sizeof(m_matrix));
		return *this;
	}
	float* const Matrix3x3::operator[](size_t column) {
		return m_matrix[column];
	}
	const float* const Matrix3x3::operator[](size_t column) const {
		return m_matrix[column];
	}
	Vector3f operator*(const Matrix3x3& left, const Vector3f& right) {
		return Vector3f(
			left[0][0] * right.x + left[1][0] * right.y + left[2][0] * right.z,
			left[0][1] * right.x + left[1][1] * right.y + left[2][1] * right.z,
			left[0][2] * right.x + left[1][2] * right.y + left[2][2] * right.z
		);
	}
	Matrix3x3 operator*(const Matrix3x3& left, const Matrix3x3& right) {

		Matrix3x3 new_object;
		Multiply(new_object.m_matrix, left.m_matrix, right.m_matrix);

		return new_object;
	}
	Matrix3x3& operator*=(Matrix3x3& left, const Matrix3x3& right) {
		Multiply(left.m_matrix, left.m_matrix, right.m_matrix);
		return left;
	}
	bool operator==(const Matrix3x3& left, const Matrix3x3& right) {
		for (unsigned char c(0); c < size; c++)
			for (unsigned char r(0); r < size; r++)
				if (left[c][r] != right[c][r])
					return false;
		return true;
	}
	bool operator!=(const Matrix3x3& left, const Matrix3x3& right) {
		return !(left == right);
	}
}