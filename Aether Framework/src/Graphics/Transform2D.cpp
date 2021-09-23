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


#include "Graphics/Transform2D.hpp"

#include <math.h>
#include <iostream>

namespace ae {

	Transform2D::Transform2D() : m_needs_update(true), m_inverse_needs_update(true), m_rotation(0.f), m_scale(1.f, 1.f), m_matrix(), m_inverse_matrix() {}

	Transform2D& Transform2D::SetOrigin(const Vector2f& origin) {
		m_origin = origin;
		m_needs_update = true;
		m_inverse_needs_update = true;
		return *this;
	}
	const Vector2f& Transform2D::GetOrigin() const {
		return m_origin;
	}

	Transform2D& Transform2D::Move(const Vector2f& offset) {
		return SetPosition(m_translation + offset);
	}
	Transform2D& Transform2D::SetPosition(const Vector2f& position) {
		m_translation = position;
		m_needs_update = true;
		m_inverse_needs_update = true;
		return *this;
	}
	const Vector2f& Transform2D::GetPosition() const {
		return m_translation;
	}

	Transform2D& Transform2D::Scale(const Vector2f& factor) {
		return SetScale( Vector2f(m_scale.x * factor.x, m_scale.y * factor.y) );
	}
	Transform2D& Transform2D::Scale(float factor) {
		return SetScale(m_scale * factor);
	}
	Transform2D& Transform2D::Scale(const Vector2f& factor, const Vector2f& center) {

		m_translation = {
			center.x + (m_translation.x - center.x) * factor.x,
			center.y + (m_translation.y - center.y) * factor.y
		};

		Scale(factor);
		return *this;
	}
	Transform2D& Transform2D::Scale(float factor, const Vector2f& center) {
		return Scale( Vector2f(factor, factor), center );
	}

	Transform2D& Transform2D::SetScale(const Vector2f& factor) {
		m_scale = factor;
		m_needs_update = true;
		m_inverse_needs_update = true;
		return *this;
	}
	Transform2D& Transform2D::SetScale(float factor) {
		return SetScale( Vector2f(factor, factor) );
	}
	const Vector2f& Transform2D::GetScale() const {
		return m_scale;
	}

	Transform2D& Transform2D::Rotate(float degrees) {
		return SetRotation(m_rotation + degrees);
	}
	Transform2D& Transform2D::Rotate(float degrees, const Vector2f& center) {
		float radians = degrees * 3.1415927f / 180.f;

		m_translation = {
			static_cast<float>(cos(radians)) * (m_translation.x - center.x) - static_cast<float>(sin(radians)) * (m_translation.y - center.y) + center.x,
			static_cast<float>(sin(radians)) * (m_translation.x - center.x) + static_cast<float>(cos(radians)) * (m_translation.y - center.y) + center.y
		};

		Rotate(degrees);

		return *this;
	}
	Transform2D& Transform2D::SetRotation(float degrees) {

		m_rotation = static_cast<float>(fmod(degrees, 360.0));

		if (m_rotation < 0)
			m_rotation += 360.f;

		m_needs_update = true;
		m_inverse_needs_update = true;
		return *this;
	}
	float Transform2D::GetRotation() const {
		return m_rotation;
	}

	Transform2D& Transform2D::Shear(const Vector2f& shear) {
		return SetShear(m_shear + shear);
	}
	Transform2D& Transform2D::SetShear(const Vector2f& shear) {
		m_shear = shear;
		m_needs_update = true;
		m_inverse_needs_update = true;
		return *this;
	}
	const Vector2f& Transform2D::GetShear() const {
		return m_shear;
	}

	void Transform2D::UpdateMatrix() const {
		if (m_needs_update) {

			// precomputed matrix multiplication
			// equivalent of: translation * origin * rotation * scale * shear * inverse_origin
			// matrix multiplication is in reverse order to combining transformations!

			float radians = m_rotation * 3.141592654f / 180.f;
			float sine = static_cast<float>(sin(radians));
			float cosine = static_cast<float>(cos(radians));

			float v00 = (  cosine * m_scale.x ) - ( sine   * m_shear.y * m_scale.y );
			float v10 = ( -sine   * m_scale.y ) + ( cosine * m_shear.x * m_scale.x );
			float v20 = -m_origin.x * (v00 - 1.f) - m_origin.y * v10 + m_translation.x;

			float v01 = (  sine   * m_scale.x ) + ( cosine * m_shear.y * m_scale.y );
			float v11 = (  cosine * m_scale.y ) + ( sine   * m_shear.x * m_scale.x );
			float v21 = -m_origin.x * v01 - m_origin.y * (v11 - 1.f) + m_translation.y;

			m_matrix = {
				v00, v10, v20,
				v01, v11, v21,
				0.f, 0.f, 1.f
			};

			m_needs_update = false;
		}
	}

	void Transform2D::UpdateInverseMatrix() const {

		if (m_inverse_needs_update) {
			UpdateMatrix();
			m_inverse_matrix = m_matrix.GetInverse();
			m_inverse_needs_update = false;
		}
	}

	Vector2f Transform2D::TransformPoint(const Vector2f& point) const {
		return GetMatrix().TransformPoint(point);
	}
	FloatRect Transform2D::TransformRectangle(const FloatRect& rectangle) const {
		return GetMatrix().TransformRectangle(rectangle);
	}

	const Matrix3x3& Transform2D::GetMatrix() const {
		UpdateMatrix();
		return m_matrix;
	}
	const Matrix3x3& Transform2D::GetInverseMatrix() const {
		UpdateInverseMatrix();
		return m_inverse_matrix;
	}
}