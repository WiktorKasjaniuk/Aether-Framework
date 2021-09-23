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

/////////////////////////////////////////////////////////////////////////////////////////////
/// Camera
///
/// Camera stores view and projection transformation matrices.
/// Note that GetViewMatrix() returns the inverse of it's transformations,
/// as the camera does not really move, it is just an offset from normal position.
/// 
/// StretchOnResize:
///		Set on true, after window resize, visible space in the window will stay the same,
///		set on false, it will adjust to the new size.
/// 
/// FixPosition:
///		Transforms the point by view matrix.
/// 
/////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Core/CreateStructure.hpp"
#include "../Graphics/Transform2D.hpp"

namespace ae {

	namespace internal {

		class ApplicationType;
		void SetEventCallbacks(void* native_window);

		class CameraType : public Transform2D {
			friend class ae::internal::ApplicationType;
			friend void ae::internal::SetEventCallbacks(void* native_window);

			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<CameraType>();

		public:
			~CameraType() = default;

			const Matrix3x3& GetViewMatrix() const;
			const Matrix3x3& GetInverseViewMatrix() const;

			const Matrix3x3& GetProjMatrix() const;
			const Matrix3x3& GetProjViewMatrix() const;

			void StretchOnResize(bool stretch = false);

			Vector2f FixPosition(const Vector2f& position) const;

		private:
			mutable Matrix3x3 m_proj;
			mutable Matrix3x3 m_proj_view;

			bool m_stretch;

			CameraType();
			CameraType(const CameraType&) = delete;
			CameraType(CameraType&&) = delete;

			void UpdateCameraSize(const Vector2i& size);

			virtual void UpdateMatrix() const override;
			using Transform2D::GetMatrix;
			using Transform2D::GetInverseMatrix;

			using Transform2D::TransformPoint;
			using Transform2D::TransformRectangle;
		};
	}

	extern ae::internal::CameraType Camera;

}