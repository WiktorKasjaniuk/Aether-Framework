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

#include "Structure/Camera.hpp"
#include "Structure/Window.hpp"

#include<iostream>

namespace ae {

	internal::CameraType Camera = internal::CreateStructure<internal::CameraType>();

	namespace internal {

		CameraType::CameraType() : Transform2D(), m_stretch(false), m_proj(), m_proj_view() {}

		const Matrix3x3& CameraType::GetViewMatrix() const {
			return GetMatrix();
		}
		const Matrix3x3& CameraType::GetInverseViewMatrix() const {
			return GetInverseMatrix();
		}

		const Matrix3x3& CameraType::GetProjMatrix() const {
			return m_proj;
		}
		const Matrix3x3& CameraType::GetProjViewMatrix() const {
			UpdateMatrix();
			return m_proj_view;
		}

		void CameraType::StretchOnResize(bool stretch) {
			m_stretch = stretch;
		}

		Vector2f CameraType::FixPosition(const Vector2f& position) const {
			const Matrix3x3& camera = GetInverseMatrix();
			Vector3f global_pos = camera * Vector3f(position.x, position.y, 1.f);

			return Vector2f(global_pos.x, global_pos.y);
		}


		void CameraType::UpdateMatrix() const {
			if (m_needs_update) {

				// precomputed matrix multiplication (for camera)
				// matrix multiplication is in reverse order to combining transformations!
				// equivalent of
				// m_matrix =
				//		ae::Matrix3x3::Identity
				//		.GetTranslated(m_translation)
				//		.GetRotated(m_rotation)
				//		.GetScaled(ae::Vector2f(1.f / m_scale.x, 1.f / m_scale.y))
				//		.GetSheared(m_shear)
				//		.GetTranslated(-m_origin)
				//		.GetInverse()
				// ;

				float radians = m_rotation * 3.1415927f / 180.f;
				float sine = static_cast<float>(sin(radians));
				float cosine = static_cast<float>(cos(radians));

				float v00 = (cosine / m_scale.x) - (sine * m_shear.y / m_scale.y);
				float v10 = (-sine / m_scale.y) + (cosine * m_shear.x / m_scale.x);
				float v20 = -m_origin.x * (v00 - 1.f) - m_origin.y * v10 + m_translation.x - m_origin.x;

				float v01 = (sine / m_scale.x) + (cosine * m_shear.y / m_scale.y);
				float v11 = (cosine / m_scale.y) + (sine * m_shear.x / m_scale.x);
				float v21 = -m_origin.x * v01 - m_origin.y * (v11 - 1.f) + m_translation.y - m_origin.y;

				m_matrix = Matrix3x3(
					v00, v10, v20,
					v01, v11, v21,
					0.f, 0.f, 1.f
				).GetInverse();

				// update projection view matrix
				m_proj_view = m_proj * m_matrix;

				m_needs_update = false;
			}
		}

		void CameraType::UpdateCameraSize(const Vector2i& size) {
			if (!m_stretch) {
				m_origin = Vector2f(size) / 2.f;
				
				m_proj = {
					2.f / size.x,  0.f,          -1.f,
					0.f,          -2.f / size.y,  1.f,
					0.f,           0.f,           1.f
				};

				m_needs_update = true;
				m_inverse_needs_update = true;
			}
		}

	}

}