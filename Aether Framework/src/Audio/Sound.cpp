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


#include "Audio/Sound.hpp"

#include "Audio/AudioDevice.hpp"

#include "Core/OpenALCalls.hpp"
#include "System/LogError.hpp"

#include <OpenALSoft/al.h>

namespace ae {

	Sound::Sound() : internal::AudioSource() {}
	Sound::Sound(const Sound& copy) : internal::AudioSource(copy) {
		if (copy.m_sound_buffer)
			SetSoundBuffer(*copy.m_sound_buffer);
	}
	Sound::Sound(Sound&& move) noexcept : internal::AudioSource(move) {
		m_sound_buffer = move.m_sound_buffer;
	}
	Sound::~Sound() {
		Stop();
	}

	void Sound::SetSoundBuffer(const SoundBuffer& sound_buffer) { 
		if (m_sound_buffer == &sound_buffer)
			return;
		
		AudioState state = GetState();
		if (state == AudioState::Playing || state == AudioState::Paused)
			Stop();

		if (sound_buffer.WasLoaded()) {

			// check if can be spatialized
			if (IsSpatialized() && !sound_buffer.IsMono()) {
				AE_WARNING("Could not spatialize sound, newly set soundbuffer is not monophonic");
				ae::LogError("Could not spatialize sound, newly set soundbuffer is not monophonic", false);
			}

			AE_AL_LOG(alSourcei(GetSourceId(), AL_BUFFER, sound_buffer.m_sound_buffer_id));
			m_sound_buffer = &sound_buffer;
		}
		else {
			AE_AL_LOG(alSourcei(GetSourceId(), AL_BUFFER, 0));
			m_sound_buffer = nullptr;
		}
	}

	void Sound::Play() {
		if (m_sound_buffer && m_sound_buffer->WasLoaded()) {

			if (GetState() == AudioState::Paused)
				Stop();

			AE_AL_LOG(alSourcePlay(GetSourceId()));
			
			// Warn if cant spatialize
			static bool spatialize_error_caught = false;
			if (IsSpatialized() && !m_sound_buffer->IsMono() && !spatialize_error_caught) {
				AE_WARNING("Could not spatialize sound, soundbuffer is not monophonic");
				ae::LogError("Could not spatialize sound, soundbuffer is not monophonic", false);
				spatialize_error_caught = true;
			}
		}
	}
	void Sound::Stop() {
		if (m_sound_buffer && m_sound_buffer->WasLoaded()) {
			AE_AL_LOG(alSourceStop(GetSourceId()));
		}
	}
	void Sound::Pause() {
		if (m_sound_buffer && m_sound_buffer->WasLoaded()) {
			AE_AL_LOG(alSourcePause(GetSourceId()));
		}
	}
	void Sound::Resume() {
		if (m_sound_buffer && m_sound_buffer->WasLoaded() && GetState() == AudioState::Paused) {
			AE_AL_LOG(alSourcePlay(GetSourceId()));
		}
	}

	void Sound::SetLoop(bool should_loop) {
		AE_AL_LOG(alSourcei(GetSourceId(), AL_LOOPING, should_loop));
	}

	AudioState Sound::GetState() const {
		std::int32_t state;
		AE_AL_LOG(alGetSourcei(GetSourceId(), AL_SOURCE_STATE, &state));

		switch (state)
		{
		default:
		case AL_INITIAL: case AL_STOPPED:
			return AudioState::Stopped;

		case AL_PLAYING:
			return AudioState::Playing;

		case AL_PAUSED:
			return AudioState::Paused;
		}
	}
	
	const SoundBuffer* Sound::GetSoundBuffer() const { return m_sound_buffer; }
	
	bool Sound::IsLooped() const {
		std::int32_t is_looped;
		AE_AL_LOG(alGetSourcei(GetSourceId(), AL_LOOPING, &is_looped));
		return (is_looped == AL_LOOPING);
	}

}