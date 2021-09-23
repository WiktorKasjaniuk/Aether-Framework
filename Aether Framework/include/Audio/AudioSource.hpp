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

#include "../System/Vector3.hpp"
#include "../System/Time.hpp"

#include <cstdint>

namespace ae {

	enum class AudioState : std::uint8_t {
		Stopped = 0,
		Playing = 1,
		Paused = 2
	};

	namespace internal {
		class AudioSource {
		public:
			~AudioSource();

			virtual void Play() = 0;
			virtual void Stop() = 0;
			virtual void Pause() = 0;
			virtual void Resume() = 0;

			void SetPitch(float pitch);
			void SetVolume(float volume);
			void SetMinVolume(float volume);
			void SetMaxVolume(float volume);

			void SetMaxDistance(float distance);
			void SetReferenceDistance(float distance);
			void SetRollofFactor(float factor);

			void SetPosition(const Vector3f& position);
			void SetVelocity(const Vector3f& velocity);
			void SetDirection(const Vector3f& direction);

			void SetConeInnerAngle(float degrees);
			void SetConeOuterAngle(float degrees);
			void SetConeOuterVolume(float volume);

			void SetTimeOffset(const Time& offset);
			void SetSpatialization(bool spatialize);

			virtual AudioState GetState() const = 0;

			float GetPitch() const;
			float GetRollofFactor() const;

			float GetVolume() const;
			float GetMinVolume() const;
			float GetMaxVolume() const;

			float GetReferenceDistance() const;
			float GetMaxDistance() const;

			Vector3f GetPosition() const;
			Vector3f GetVelocity() const;
			Vector3f GetDirection() const;

			float GetConeInnerAngleDegrees() const;
			float GetConeOuterAngleDegrees() const;
			float GetConeOuterVolume() const;

			Time GetTimeOffset() const;
			bool IsSpatialized() const;

		protected:
			AudioSource();
			AudioSource(AudioSource&& move) noexcept;
			AudioSource(const AudioSource& copy);

			std::uint32_t GetSourceId() const;

		private:
			std::uint32_t m_sound_source_id = 0;
		};
	}
}