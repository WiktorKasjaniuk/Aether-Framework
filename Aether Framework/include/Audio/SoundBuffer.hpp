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

//////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// SoundBuffer
///
/// General Idea:
///		Soundbuffer holds the information needed to play a sound.
/// 
///	Loading:
///		SB can be loaded from file or directly from samples:
///			- const float* samples are normalized samples [-1.f, 1.f],
///			- const std::int16_t* samples are between [-32768, 32767].
/// 
///		if channel_count == 1, the internal type will be mono,
///		if channel_count == 2, stereo,
///		if channel_count >= 3, the LoadFromFle() will return false and not load the SoundBuffer.
///		Samples are always stored in 16 bits.
/// 
///		if cache_samples == true, the soundbuffer data will be cached, and optainable via GetSamples(), 
///		if cache_samples == false, GetSamples() will return nullptr.
/// 
/// Saving to File:
///		if samples were cached, it is possible to save the data to a file,
///		SaveToFile() returns true if the operation was successful.
/// 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../System/Time.hpp"

#include <vector>
#include <string>

namespace ae {
	class SoundBuffer;

	bool operator==(const SoundBuffer& left, const SoundBuffer& right);
	bool operator!=(const SoundBuffer& left, const SoundBuffer& right);

	class SoundBuffer {
		friend class Sound;

		friend bool operator==(const SoundBuffer& left, const SoundBuffer& right);
		friend bool operator!=(const SoundBuffer& left, const SoundBuffer& right);

	public:
		SoundBuffer() = default;
		SoundBuffer(SoundBuffer&& move) noexcept;
		SoundBuffer(const SoundBuffer&) = delete;
		~SoundBuffer();

		bool LoadFromFile(const std::string& filename, bool cache_samples = false);
		void LoadFromSamples(const float* samples, std::int64_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples);
		void LoadFromSamples(const std::int16_t* samples, std::int64_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples);

		bool SaveToFile(const std::string& filename) const;

		bool WasLoaded() const;

		const std::int16_t* GetSamples() const;
		std::int64_t GetSampleCount() const;
		std::int64_t GetFrameCount() const;
		std::int32_t GetSampleRate() const;
		std::uint8_t GetChannelCount() const;
		bool IsMono() const;
		Time GetDuration() const;

	private:
		std::uint32_t m_sound_buffer_id = 0;

		const std::int16_t* m_samples = nullptr;

		std::int64_t m_sample_count = 0;
		std::int32_t m_sample_rate = 0;
		std::uint8_t m_channel_count = 0;

		void CacheAndCreate(const std::int16_t* samples, size_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples);
	};
}