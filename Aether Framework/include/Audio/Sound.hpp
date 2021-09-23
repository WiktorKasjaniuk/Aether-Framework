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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Sound
///
/// General Idea:
///		A class to use sound effects, Sound and SoundBuffers are like Sprite and Textures.
///		It also can spatialize mono sounds.
/// 
/// Playing:
///		if a SoundBuffer was set, now the sound effect can be played,
///		
///		Play() - restarts sound if needed, and plays the sound,
///		Stop() - stops the sounds,
///		Pause() - pauses the sound if needed,
///		Resume() - resumes the sound if it was paused,
/// 
///		SetLoop(loop) - determines if the sound should automatically restart after it's finished.
/// 
/// Attributes:
///		Pitch - sound's height,
///		Volume - base gain (defaultly 1.f),
///		TimeOffset - time offset into the sound effect.
/// 
/// Spatialization:
///		Spatialization is a process of placing a sound in a 3D space / 2D plane,
///		by adjusting gain to both left and right speakers and / or pitch by simulating Doppler's shift.
///		All spatialized sounds are relative to Listener.
///		
/// Spatialization Attributes:
///		Only mono (1 channel) SoundBuffers can be spatialized and
///		SetSpatialization(true) is needed to begin the process.
/// 
///		Position - position in 3D world:
///			z coordinate is needed to split a sound between left and right headphones,
/// 
///		Velocity - needed to simulate Doppler's shift,
///		Direction - the direction in which sound is playing, values: {-1.f, 0.f, 1.f}, default {0.f, 0.f, 0.f},
/// 
///		MinVolume - minimum gain, the sound can reach via spatialization,	
///		MaxVolume - maximum gain, the sound can reach via spatialization,	
///		
///		SetRollofFactor - decides how quickly the gain decreases while distance increases,
///		ReferenceDistance - 
///			distance to listener, in which sound effect is played at Volume,
/// 
///			if listener's sound model is set to clamped:
///				gain will be clamped to Volume before this distance,
///			else:
///				gain will decrease below Volume,
/// 
///			either way, if the distance is higher, the gain will also be higher,
/// 
///		SetMaxDistance - 
///			if listener's sound model is set to clamped:
///				it decides the distance to listener, in which gain is the highest,
/// 
///		Cone:
///			Sounds can be made in 360 degrees, but also in lower values, creating a cone heading the Direction of sound.
///			Thus calculate the volume ConeInnerAngle, ConeOuterAngle and ConeOuterVolume are used.
///			ConeOuterVolume must be between [0.f, 1.f].
///
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "SoundBuffer.hpp"
#include "AudioSource.hpp"

namespace ae {
	namespace internal { class AudioManager; }

	class Sound : public internal::AudioSource {
		friend class internal::AudioManager;

	public:
		Sound();
		Sound(const Sound& copy);
		Sound(Sound&& move) noexcept;
		~Sound();

		void SetSoundBuffer(const SoundBuffer& sound_buffer);
		const SoundBuffer* GetSoundBuffer() const;

		virtual void Play() override;
		virtual void Stop() override;
		virtual void Pause() override;
		virtual void Resume() override;

		virtual AudioState GetState() const override;

		void SetLoop(bool should_loop);
		bool IsLooped() const;

	private:
		const SoundBuffer* m_sound_buffer = nullptr;
	};
}