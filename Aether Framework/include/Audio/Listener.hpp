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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Listener
///
///	General Idea:
///		Just a camera, but for hearing, this singleton represents the ears of application.
/// 
/// Attributes:
///		GlobalVolume - volume of all played sounds,
/// 
/// Spatialization:
///		
///		SetDistanceModel -
///			used to calculate the output gain of all spatialized sounds,
///			clamped versions have minimal and maximum sound volume set by the Sound class.
///			Default value is InverseClamped.
/// 
///			Possible values and their formulas using Sound's attributes:	
///			(Search 'OpenAL Distance Models' or 'OpenAL Distance Attetuation' for graphical explenation)
///			
///			None:
///				gain = 0.f;
/// 
///			Inverse:
///				gain = ReferenceDistance / (ReferenceDistance + RollofFactor * (distance – ReferenceDistance)); 
/// 
///			InverseClamped:
///				distance = max(distance, ReferenceDistance);
///				distance = min(distance, MaxDistance);
///				gain = ReferenceDistance / (ReferenceDistance + RollofFactor * (distance – ReferenceDistance)); 
/// 
///			Linear:
///				distance = min(distance, MaxDistance);
///				gain = (1.f – RollofFactor * (distance – ReferenceDistance) / (MaxDistance – ReferenceDistance));
/// 
///			LinearClamped:
///				distance = max(distance, ReferenceDistance);
///				distance = min(distance, MaxDistance);
///				gain = (1.f – RollofFactor * (distance – ReferenceDistance) / (MaxDistance – ReferenceDistance));
/// 
///			Exponent:
///				gain = (distance / ReferenceDistance) ^ (-RollofFactor);
/// 
///			ExponentClamped:
///				distance = max(distance, ReferenceDistance);
///				distance = min(distance, MaxDistance);
///				gain = (distance / ReferenceDistance) ^ (- RollofFactor);
/// 
///		Position - Listener's position in 3D space / 2D plane,
///		UpVector - sets coordinate system for the Listener, values: { -1.f / 0.f / 1.f }, default = Vec3(0.f, 1.f, 0.f)
///		Direction - also used for ccalculations, values: { -1.f / 0.f / 1.f }, default = Vec3(0.f, 0.f, -1.f).
///			
///		Doppler's Shift:
///			DopplerFactor - determines the strength of Doppler's shift (default is 1.f),
///			SoundSpeed - determines the velocity of sound in the medium (default is 343.3f),
/// 
///			Velocity - used for Doppler's shift calculations,
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "../Core/CreateStructure.hpp"
#include "../System/Vector3.hpp"

#include <utility>

namespace ae {
	enum class ListenerDistanceModel {
		None = 0x0000,

		Inverse = 0xD001,
		Exponent = 0xD005,
		Linear = 0xD003,

		InverseClamped = 0xD002,
		ExponentClamped = 0x0D006,
		LinearClamped = 0xD004
	};

	namespace internal {
		class AudioDevice;
		class ListenerType {
			friend class AudioDevice;

			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<ListenerType>();

		public:
			~ListenerType() = default;

			void SetGlobalVolume(float volume);

			void SetPosition(const Vector3f& position);
			void SetVelocity(const Vector3f& velocity);
			void SetDirection(const Vector3f& direction = Vector3f(0.f, 0.f, -1.f));
			void SetUpVector(const Vector3f& up_vector = Vector3f(0.f, 1.f, 0.f));

			void SetDistanceModel(ae::ListenerDistanceModel model = ae::ListenerDistanceModel::InverseClamped);
			void SetDopplerFactor(float value = 1.f);
			void SetSoundSpeed(float value = 343.3f);

			float GetGlobalVolume() const;
			Vector3f GetPosition() const;
			Vector3f GetVelocity() const;
			const Vector3f& GetDirection() const;
			const Vector3f& GetUpVector() const;

		private:
			Vector3f m_direction, m_up_vector;

			ListenerType() = default;
			ListenerType(const ListenerType&) = delete;
			ListenerType(ListenerType&&) = delete;

			void Initialize();
		};
	}

	extern ae::internal::ListenerType Listener;
}