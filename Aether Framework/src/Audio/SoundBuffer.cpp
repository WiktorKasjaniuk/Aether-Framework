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


#include "Audio/SoundBuffer.hpp"

#include "Core/OpenALCalls.hpp"
#include "Core/Preprocessor.hpp"

#include <OpenALSoft/al.h>
#include <sndfile/sndfile.h>

namespace ae {

	SoundBuffer::SoundBuffer(SoundBuffer&& move) noexcept {
		move.m_sound_buffer_id = m_sound_buffer_id;
		move.m_samples = m_samples;
		move.m_sample_count = m_sample_count;
		move.m_sample_rate = m_sample_rate;
		move.m_channel_count = m_channel_count;

		m_sound_buffer_id = 0;
		m_samples = nullptr;
	}
	SoundBuffer::~SoundBuffer() {
		AE_AL_LOG(alDeleteBuffers(1, &m_sound_buffer_id));

		if (m_samples)
			delete[] m_samples;
	}

	const std::int16_t* SoundBuffer::GetSamples() const { return m_samples; }
	std::int64_t SoundBuffer::GetSampleCount() const { return m_sample_count; }
	std::int64_t SoundBuffer::GetFrameCount() const { return m_sample_count / m_channel_count; }
	std::int32_t SoundBuffer::GetSampleRate() const { return m_sample_rate; }
	std::uint8_t SoundBuffer::GetChannelCount() const { return m_channel_count; }
	bool SoundBuffer::IsMono() const { return (m_channel_count == 1); }
	Time SoundBuffer::GetDuration() const {
		return Time(static_cast<double>(m_sample_count) / m_sample_rate / m_channel_count);
	}

	bool SoundBuffer::WasLoaded() const { return (m_sound_buffer_id != 0); }
	bool SoundBuffer::LoadFromFile(const std::string& filename, bool cache_samples) {

		AE_ASSERT(!WasLoaded(), "Soundbuffer was already loaded");

		// load
		SF_INFO audio_data;
		SNDFILE* file = sf_open(filename.c_str(), SFM_READ, &audio_data);

		if (!file || audio_data.frames < 1 || audio_data.frames > static_cast<sf_count_t>(INT_MAX / sizeof(short)) / audio_data.channels) {
			AE_WARNING("Could not load soundbuffer from file '" << filename << "', invalid path");
			return false;
		}

		if (audio_data.channels > 2) {
			AE_WARNING("Could not load soundbuffer from file '" << filename << "', 2 channels limit exceeded, count: '" << audio_data.channels << '\'');
			return false;
		}

		// read samples
		std::int16_t* samples = new std::int16_t[audio_data.frames * audio_data.channels * sizeof(short)];
		std::int64_t frame_count = sf_readf_short(file, samples, audio_data.frames);
		sf_close(file);

		if (frame_count < 1) {
			AE_WARNING("Could not load soundbuffer from file '" << filename << "', invalid frame count");
			delete[] samples;
			return false;
		}

		// forward
		CacheAndCreate(samples, audio_data.frames * audio_data.channels, audio_data.channels, audio_data.samplerate, cache_samples);

		// clean up
		if (!cache_samples)
			delete[] samples;

		return true;
	}
	void SoundBuffer::LoadFromSamples(const float* samples, std::int64_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples) {
		AE_ASSERT(!WasLoaded(), "Soundbuffer was already loaded");
		
		// convert data from [-1.f, 1.f] to [-32768, 32767]
		constexpr double min_value = std::numeric_limits<std::int16_t>::min();
		constexpr double max_value = std::numeric_limits<std::int16_t>::max();
		constexpr double multiplier = (max_value - min_value) / 2.0;

		std::int16_t* samples_16bit = new std::int16_t[sample_count];

		for (size_t sample_index(0); sample_index < sample_count; sample_index++) {

			// fix the sample
			double sample = std::max(std::min(static_cast<double>(samples[sample_index]), 1.0), -1.0);

			// convert to short
			samples_16bit[sample_index] = static_cast<std::int16_t>(sample * multiplier);
		}

		// forward
		CacheAndCreate(samples_16bit, sample_count, channel_count, sample_rate, cache_samples);

		// clean up
		if (!cache_samples)
			delete[] samples_16bit;
	}

	void SoundBuffer::LoadFromSamples(const std::int16_t* samples, std::int64_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples) {
		AE_ASSERT(!WasLoaded(), "Soundbuffer was already loaded");

		// copy samples if needed
		if (cache_samples) {
			std::int16_t* _samples = new std::int16_t[sample_count];
			memcpy(_samples, samples, sizeof(std::int16_t) * sample_count);
			CacheAndCreate(_samples, sample_count, channel_count, sample_rate, cache_samples);
		}
		else
			CacheAndCreate(samples, sample_count, channel_count, sample_rate, cache_samples);
	}

	void SoundBuffer::CacheAndCreate(const std::int16_t* samples, size_t sample_count, std::uint8_t channel_count, std::int32_t sample_rate, bool cache_samples) {

		// create OpenAL buffer
		AE_AL_LOG(alGenBuffers(1, &m_sound_buffer_id));
		AE_AL_LOG(
			alBufferData(
				m_sound_buffer_id,
				channel_count == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16,
				samples,
				sizeof(std::int16_t) * sample_count,
				sample_rate
			)
		);

		// cache
		if (cache_samples)
			m_samples = samples;

		m_sample_count = sample_count;
		m_sample_rate = sample_rate;
		m_channel_count = channel_count;
	}

	bool SoundBuffer::SaveToFile(const std::string& filename) const {

		// check if loaded
		if (!m_samples) {
			AE_WARNING("Could not save soundbuffer to file '" << filename << "', either soundbuffer not loaded or samples not cached");
			return false;
		}

		// upload data
		SF_INFO audio_data;
		audio_data.channels = m_channel_count;
		audio_data.samplerate = m_sample_rate;

		// figure out format
		size_t dot_pos = filename.rfind('.');

		if (dot_pos == filename.npos || dot_pos == filename.size() - 1) {
			AE_WARNING("Could not save soundbuffer to file '" << filename << "', format not found");
			return false;
		}

		std::string format_string = filename.substr(dot_pos + 1);
		for (char& c : format_string)
			c = std::tolower(c);

		// convert format to enum
#define SET_FORMAT_IF1(format, format_string, format_enum, str1)				else if (format_string == str1) format = format_enum | SF_FORMAT_PCM_16
#define SET_FORMAT_IF2(format, format_string, format_enum, str1, str2)			else if (format_string == str1 || format_string == str2) format = format_enum | SF_FORMAT_PCM_16
#define SET_FORMAT_IF3(format, format_string, format_enum, str1, str2, str3)	else if (format_string == str1 || format_string == str2 || format_string == str3) format = format_enum | SF_FORMAT_PCM_16

		if (false); // for consistency

		SET_FORMAT_IF2(audio_data.format, format_string, SF_FORMAT_WAV,		"wav", "wave");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_AIFF,	"aiff");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_AU,		"au");
		SET_FORMAT_IF2(audio_data.format, format_string, SF_FORMAT_RAW,		"raw", "pcm");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_PAF,		"paf");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_SVX,		"svx");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_FLAC,	"flac");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_WVE,		"wve");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_OGG,		"ogg");
		SET_FORMAT_IF1(audio_data.format, format_string, SF_FORMAT_VOC,		"voc");

		else {
			AE_WARNING("Could not save soundbuffer to file '" << filename << "', invalid format");
			return false;
		}

		// write to file
		SNDFILE* file = sf_open(filename.c_str(), SFM_WRITE, &audio_data);

		if (!file) {
			AE_WARNING("Could not save soundbuffer to file '" << filename << "', invalid filename");
			return false;
		}
		std::int64_t count = sf_write_short(file, m_samples, m_sample_count);

		sf_write_sync(file);
		sf_close(file);

		return true;
	}

	bool operator==(const SoundBuffer& left, const SoundBuffer& right) {
		return (left.m_sound_buffer_id == right.m_sound_buffer_id);
	}
	bool operator!=(const SoundBuffer& left, const SoundBuffer& right) {
		return (left.m_sound_buffer_id != right.m_sound_buffer_id);
	}
}