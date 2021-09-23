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


#include "Audio/Listener.hpp"

#include "Core/OpenALCalls.hpp"

#include <OpenALSoft/al.h>

namespace ae {
	internal::ListenerType Listener = internal::CreateStructure<internal::ListenerType>();

	namespace internal {
		
		void ListenerType::Initialize() {
			SetDirection(Vector3f(0.f, 0.f, -1.f));
			SetUpVector(Vector3f(0.f, 1.f, 0.f));
		}

		void ListenerType::SetGlobalVolume(float volume) {
			AE_AL_LOG(alListenerf(AL_GAIN, std::max(volume, 0.f)));
		}

		void ListenerType::SetDistanceModel(ae::ListenerDistanceModel model) {
			AE_AL_LOG(alDistanceModel(static_cast<ALenum>(model)));
		}
		void ListenerType::SetDopplerFactor(float value) {
			AE_AL_LOG(alDopplerFactor(std::max(value, 0.f)));
		}
		void ListenerType::SetSoundSpeed(float value) {
			AE_AL_LOG(alSpeedOfSound(std::max(value, 0.f)));
		}

		void ListenerType::SetPosition(const Vector3f& position) {
			AE_AL_LOG(alListener3f(AL_POSITION, position.x, position.y, position.z));
		}
		void ListenerType::SetVelocity(const Vector3f& velocity) {
			AE_AL_LOG(alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z));
		}
		void ListenerType::SetDirection(const Vector3f& direction) {
			m_direction = direction;

			Vector3f vectors[] = { m_direction, m_up_vector };
			AE_AL_LOG(alListenerfv(AL_ORIENTATION, reinterpret_cast<float*>(vectors)));
		}
		void ListenerType::SetUpVector(const Vector3f& up_vector) {
			m_up_vector = up_vector;

			Vector3f vectors[] = { m_direction, m_up_vector };
			AE_AL_LOG(alListenerfv(AL_ORIENTATION, reinterpret_cast<float*>(vectors)));
		}
		float ListenerType::GetGlobalVolume() const {
			float volume;
			AE_AL_LOG(alGetListenerf(AL_GAIN, &volume));
			return volume;
		}

		Vector3f ListenerType::GetPosition() const {
			Vector3f position;
			AE_AL_LOG(alGetListener3f(AL_POSITION, &position.x, &position.y, &position.z));
			return std::move(position);
		}
		Vector3f ListenerType::GetVelocity() const {
			Vector3f velocity;
			AE_AL_LOG(alGetListener3f(AL_POSITION, &velocity.x, &velocity.y, &velocity.z));
			return std::move(velocity);
		}
		const Vector3f& ListenerType::GetDirection() const { return m_direction; }
		const Vector3f& ListenerType::GetUpVector() const { return m_up_vector; }
	}
}