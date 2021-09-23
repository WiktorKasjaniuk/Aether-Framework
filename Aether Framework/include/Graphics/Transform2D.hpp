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

//////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Transform2D
///
/// Transform2D is a class managing Matrix3x3 and it's inverse,
/// parameters can be changed at any moment,
/// they will always be end up combined the same way:
///		> shear,
///		> scale,
///		> rotate,
///		> move by origin,
///		> translate.
/// 
/// Scaling and rotating around a point will also change the translation.
/// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "Matrix3x3.hpp"
#include "../System/Vector2.hpp"
#include "../System/Rectangle.hpp"

namespace ae {

	class Transform2D {
	public:
		Transform2D();

		// Origin
		Transform2D& SetOrigin(const Vector2f& origin);
		const Vector2f& GetOrigin() const;

		// Translation
		Transform2D& Move(const Vector2f& offset);
		Transform2D& SetPosition(const Vector2f& position);
		const Vector2f& GetPosition() const;

		// Scale
		Transform2D& Scale(const Vector2f& factor);
		Transform2D& Scale(float factor);

		Transform2D& Scale(const Vector2f& factor, const Vector2f& center);
		Transform2D& Scale(float factor,           const Vector2f& center);

		Transform2D& SetScale(const Vector2f& factor);
		Transform2D& SetScale(float factor);

		const Vector2f& GetScale() const;

		// Rotation
		Transform2D& Rotate(float degrees);
		Transform2D& Rotate(float degrees, const Vector2f& center);

		Transform2D& SetRotation(float degrees);
		float GetRotation() const;

		// Shear
		Transform2D& Shear(const Vector2f& shear);
		Transform2D& SetShear(const Vector2f& shear);
		const Vector2f& GetShear() const;

		// Transform Objects
		Vector2f TransformPoint(const Vector2f& point) const;
		FloatRect TransformRectangle(const FloatRect& rectangle) const;

		// Matrices
		const Matrix3x3& GetMatrix() const;
		const Matrix3x3& GetInverseMatrix() const;

	protected:
		virtual void UpdateMatrix() const;
		virtual void UpdateInverseMatrix() const;

		Vector2f m_translation;
		Vector2f m_origin;
		float    m_rotation;
		Vector2f m_scale;
		Vector2f m_shear;
		
		mutable Matrix3x3 m_matrix;
		mutable bool      m_needs_update;

		mutable Matrix3x3 m_inverse_matrix;
		mutable bool      m_inverse_needs_update;
	};

}