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


#include "Audio/AudioSource.hpp"

#include "Core/OpenALCalls.hpp"
#include "System/LogError.hpp"

#include <OpenALSoft/al.h>

namespace ae {
	namespace internal {

		AudioSource::AudioSource() {
			AE_AL_LOG(alGenSources(1, &m_sound_source_id));
		}
		AudioSource::AudioSource(AudioSource&& move) noexcept {
			m_sound_source_id = move.m_sound_source_id;
			move.m_sound_source_id = 0;
		}
		AudioSource::AudioSource(const AudioSource& copy) {
			AE_AL_LOG(alGenSources(1, &m_sound_source_id));

			SetPitch(copy.GetPitch());
			SetRollofFactor(copy.GetRollofFactor());

			SetVolume(copy.GetVolume());
			SetMinVolume(copy.GetMinVolume());
			SetMaxVolume(copy.GetMaxVolume());

			SetMaxDistance(copy.GetMaxDistance());
			SetReferenceDistance(copy.GetReferenceDistance());

			SetPosition(copy.GetPosition());
			SetVelocity(copy.GetVelocity());
			SetDirection(copy.GetDirection());

			SetTimeOffset(copy.GetTimeOffset());
			SetSpatialization(copy.IsSpatialized());

			SetConeInnerAngle(copy.GetConeInnerAngleDegrees());
			SetConeOuterAngle(copy.GetConeOuterAngleDegrees());
			SetConeOuterVolume(copy.GetConeOuterVolume());
		}
		AudioSource::~AudioSource() {
			AE_AL_LOG(alDeleteSources(1, &m_sound_source_id));
		}

		std::uint32_t AudioSource::GetSourceId() const { return m_sound_source_id; }

		// SET
#define AE_SOUND_SET_F_LIM_0(function_name, argument, parameter) \
	void AudioSource::function_name(float argument) { \
		AE_AL_LOG(alSourcef(m_sound_source_id, parameter, std::max(argument, 0.f))); \
	}

		AE_SOUND_SET_F_LIM_0(SetPitch, pitch, AL_PITCH);
		AE_SOUND_SET_F_LIM_0(SetRollofFactor, factor, AL_ROLLOFF_FACTOR);

		AE_SOUND_SET_F_LIM_0(SetVolume, volume, AL_GAIN);
		AE_SOUND_SET_F_LIM_0(SetMinVolume, volume, AL_MIN_GAIN);
		AE_SOUND_SET_F_LIM_0(SetMaxVolume, volume, AL_MAX_GAIN);

		AE_SOUND_SET_F_LIM_0(SetReferenceDistance, distance, AL_REFERENCE_DISTANCE);
		AE_SOUND_SET_F_LIM_0(SetMaxDistance, distance, AL_MAX_DISTANCE);

		void AudioSource::SetConeInnerAngle(float degrees) {
			degrees = static_cast<float>(fmod(degrees, 360.0));
			if (degrees < 0) degrees += 360.f;

			AE_AL_LOG(alSourcef(m_sound_source_id, AL_CONE_INNER_ANGLE, degrees));
		}
		void AudioSource::SetConeOuterAngle(float degrees) {
			degrees = static_cast<float>(fmod(degrees, 360.0));
			if (degrees < 0) degrees += 360.f;

			AE_AL_LOG(alSourcef(m_sound_source_id, AL_CONE_OUTER_ANGLE, degrees));
		}
		void AudioSource::SetConeOuterVolume(float volume) {
			AE_AL_LOG(alSourcef(m_sound_source_id, AL_CONE_OUTER_GAIN, std::min(std::max(volume, 0.f), 1.f)));
		}

		void AudioSource::SetPosition(const Vector3f& position) {
			AE_AL_LOG(alSource3f(m_sound_source_id, AL_POSITION, position.x, position.y, position.z));
		}
		void AudioSource::SetVelocity(const Vector3f& velocity) {
			AE_AL_LOG(alSource3f(m_sound_source_id, AL_VELOCITY, velocity.x, velocity.y, velocity.z));
		}
		void AudioSource::SetDirection(const Vector3f& direction) {
			AE_AL_LOG(alSource3f(m_sound_source_id, AL_DIRECTION, direction.x, direction.y, direction.z));
		}
		void AudioSource::SetTimeOffset(const Time& offset) {
			AE_AL_LOG(alSourcef(GetSourceId(), AL_SEC_OFFSET, std::max(offset.GetSeconds(), 0.0)));
		}
		void AudioSource::SetSpatialization(bool spatialize) {
			AE_AL_LOG(alSourcei(m_sound_source_id, AL_SOURCE_RELATIVE, spatialize));
		}

		// GET

#define AE_SOUND_GET_F(function_name, parameter) \
	float AudioSource::function_name() const { \
		float _value; \
		AE_AL_LOG(alGetSourcef(m_sound_source_id, parameter, &_value)); \
		return _value; \
	}

		AE_SOUND_GET_F(GetPitch, AL_PITCH);
		AE_SOUND_GET_F(GetRollofFactor, AL_ROLLOFF_FACTOR);

		AE_SOUND_GET_F(GetVolume, AL_GAIN);
		AE_SOUND_GET_F(GetMinVolume, AL_MIN_GAIN);
		AE_SOUND_GET_F(GetMaxVolume, AL_MAX_GAIN);

		AE_SOUND_GET_F(GetReferenceDistance, AL_REFERENCE_DISTANCE);
		AE_SOUND_GET_F(GetMaxDistance, AL_MAX_DISTANCE);

		AE_SOUND_GET_F(GetConeInnerAngleDegrees, AL_CONE_INNER_ANGLE);
		AE_SOUND_GET_F(GetConeOuterAngleDegrees, AL_CONE_OUTER_ANGLE);
		AE_SOUND_GET_F(GetConeOuterVolume, AL_CONE_OUTER_GAIN);

		Vector3f AudioSource::GetPosition() const {
			Vector3f position;
			AE_AL_LOG(alGetSource3f(m_sound_source_id, AL_POSITION, &position.x, &position.y, &position.z));
			return std::move(position);
		}
		Vector3f AudioSource::GetVelocity() const {
			Vector3f velocity;
			AE_AL_LOG(alGetSource3f(m_sound_source_id, AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z));
			return std::move(velocity);
		}
		Vector3f AudioSource::GetDirection() const {
			Vector3f direction;
			AE_AL_LOG(alGetSource3f(m_sound_source_id, AL_DIRECTION, &direction.x, &direction.y, &direction.z));
			return std::move(direction);
		}
		Time AudioSource::GetTimeOffset() const {
			float offset;
			AE_AL_LOG(alGetSourcef(m_sound_source_id, AL_SEC_OFFSET, &offset));
			return Time(static_cast<double>(offset));
		}
		bool AudioSource::IsSpatialized() const {
			std::int32_t is_relative;
			AE_AL_LOG(alGetSourcei(m_sound_source_id, AL_SOURCE_RELATIVE, &is_relative));
			return (is_relative == AL_TRUE);
		}
	}
}