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


#include "Audio/Music.hpp"

#include "Core/OpenALCalls.hpp"
#include "System/LogError.hpp"

#include <OpenALSoft/al.h>
#include <sndfile/sndfile.h>

constexpr size_t c_frames_per_buffer = 8192;

namespace ae {
	std::thread Music::s_streaming_thread { &Music::UpdateTracks };

	std::mutex Music::s_streaming_mutex {};
	std::vector<Music*> Music::s_streaming_tracks {};

	Music::Music() : internal::AudioSource() {
		AE_AL_LOG(alGenBuffers(c_music_buffer_count, m_sound_buffer_ids));
	}
	Music::Music(const Music& copy) : AudioSource(copy) {
		AE_AL_LOG(alGenBuffers(c_music_buffer_count, m_sound_buffer_ids));
		SetFile(copy.m_filename);
	}
	Music::~Music() {
		Stop();
		AE_AL_LOG(alDeleteBuffers(c_music_buffer_count, m_sound_buffer_ids));
	}

	void Music::Play() {

		// wait for streaming thread to finish an iteration
		std::lock_guard lock(s_streaming_mutex);

		RewindStreaming();
	}
	void Music::Stop() {

		// wait for streaming thread to finish an iteration
		std::lock_guard lock(s_streaming_mutex);

		// stop playing
		if (GetRealTimeState() != AudioState::Stopped) {
			AE_AL_LOG(alSourceStop(GetSourceId()));
			AE_AL_LOG(alSourcei(GetSourceId(), AL_BUFFER, 0));
		}

		StopStreaming();
	}
	void Music::Pause() {

		// wait for streaming thread to finish an iteration
		std::lock_guard lock(s_streaming_mutex);

		if (GetRealTimeState() == AudioState::Playing) {
			AE_AL_LOG(alSourcePause(GetSourceId()));
		}
	}
	void Music::Resume() {

		// wait for streaming thread to finish an iteration
		std::lock_guard lock(s_streaming_mutex);

		if (GetRealTimeState() == AudioState::Paused) {
			AE_AL_LOG(alSourcePlay(GetSourceId()));
		}
	}

	void Music::RewindStreaming() {

		// clean up
		DiscardOldData();

		// open file
		SF_INFO audio_data;
		SNDFILE* file = sf_open(m_filename.c_str(), SFM_READ, &audio_data);
		m_native_file = file;

		if (!file || audio_data.frames < 1) {
			AE_WARNING("Could not start music stream from file '" << m_filename << "', file missing or invalid format");
			return;
		}

		if (audio_data.channels > 2) {
			AE_WARNING("Could not start music stream from file '" << m_filename << "', 2 channels limit exceeded, count: '" << audio_data.channels << '\'');
			return;
		}

		// load data
		m_sample_rate = audio_data.samplerate;
		m_channel_count = audio_data.channels;
		m_format = audio_data.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

		// rewind
		AE_AL_LOG(alSourceRewind(GetSourceId()));
		AE_AL_LOG(alSourcei(GetSourceId(), AL_BUFFER, 0));

		// load first buffers
		// 'c_frames_per_buffer' frames ('c_frames_per_buffer * channels' samples) per buffer
		m_stream_samples = new std::int16_t[c_frames_per_buffer * m_channel_count];

		for (size_t buffer_index(0); buffer_index < c_music_buffer_count; buffer_index++) {

			// read data
			std::int64_t read_frames = sf_readf_short(file, m_stream_samples, c_frames_per_buffer);

			if (read_frames < 1)
				break;

			// fill buffer
			AE_AL_LOG(
				alBufferData(
					m_sound_buffer_ids[buffer_index],
					m_format,
					m_stream_samples,
					read_frames * m_channel_count * sizeof(std::int16_t),
					m_sample_rate
				)
			);
		}

		// queue and play
		AE_AL_LOG(alSourceQueueBuffers(GetSourceId(), c_music_buffer_count, m_sound_buffer_ids));
		AE_AL_LOG(alSourcePlay(GetSourceId()));

		// dispatch for streaming
		if (std::find(s_streaming_tracks.begin(), s_streaming_tracks.end(), this) == s_streaming_tracks.end())
			s_streaming_tracks.push_back(this);
	}
	void Music::StopStreaming() {
		
		// clean up
		DiscardOldData();

		// dispatch from streaming
		auto streaming_tracks_it = std::find(s_streaming_tracks.begin(), s_streaming_tracks.end(), this);
		if (streaming_tracks_it != s_streaming_tracks.end())
			s_streaming_tracks.erase(streaming_tracks_it);
	}
	void Music::DiscardOldData() {

		// close file
		if (m_native_file) {
			sf_close(static_cast<SNDFILE*>(m_native_file));
			m_native_file = nullptr;
		}

		// delete old samples
		if (m_stream_samples) {
			delete[] m_stream_samples;
			m_stream_samples = nullptr;
		}
	}

	void Music::UpdateStream() {

		// pop, fill and push processed buffers
		std::int32_t processed;
		AE_AL_LOG(alGetSourcei(GetSourceId(), AL_BUFFERS_PROCESSED, &processed));

		while (processed > 0) {

			RequeueProcessedBuffer();

			// ensure source has not finished playing
			if (GetRealTimeState() == AudioState::Stopped) {

				std::int32_t queue_size;
				AE_AL_LOG(alGetSourcei(GetSourceId(), AL_BUFFERS_QUEUED, &queue_size));

				// track has finished
				if (queue_size == 0) {
					StopStreaming();

					if (m_loops)
						RewindStreaming();
				}

				// source finished playing before next stream
				else {
					AE_AL_LOG(alSourcePlay(GetSourceId()));
				}
			}

			// update processed buffer count & ensure other buffer has not been processed
			AE_AL_LOG(alGetSourcei(GetSourceId(), AL_BUFFERS_PROCESSED, &processed));
		}
	}

	void Music::RequeueProcessedBuffer() {

		// pop buffer
		std::uint32_t poped_buffer;
		AE_AL_LOG(alSourceUnqueueBuffers(GetSourceId(), 1, &poped_buffer));

		// fill buffer
		std::int64_t read_frames = sf_readf_short(static_cast<SNDFILE*>(m_native_file), m_stream_samples, c_frames_per_buffer);

		if (read_frames > 0) {
			AE_AL_LOG(
				alBufferData(
					poped_buffer,
					m_format,
					m_stream_samples,
					read_frames * m_channel_count * sizeof(std::int16_t),
					m_sample_rate
				)
			);

			// push buffer
			AE_AL_LOG(alSourceQueueBuffers(GetSourceId(), 1, &poped_buffer));
		}
	}

	void Music::UpdateTracks() {
		while (internal::AudioDevice::IsRunning()) {

			// lock mutex for this iteration
			std::lock_guard lock(s_streaming_mutex);

			// safty copy tracks in case they stop playing and get removed from vector
			std::vector<Music*> copied_tracks = s_streaming_tracks;

			for (Music* track : copied_tracks)
				track->UpdateStream();
		}
	}

	void Music::SetFile(const std::string& filename) { 
		AE_ASSERT(!m_native_file, "Cannot set music filename while streaming");
		m_filename = filename; 
	}
	const std::string& Music::GetFilename() const { return m_filename; }

	bool operator==(const Music& left, const Music& right) { return (left.GetSourceId() == right.GetSourceId()); }
	bool operator!=(const Music& left, const Music& right) { return (left.GetSourceId() != right.GetSourceId()); }

	void Music::SetLoop(bool should_loop) {

		// wait for streaming thread to finish an iteration
		std::lock_guard lock(s_streaming_mutex);

		m_loops = should_loop;
	}
	bool Music::IsLooped() const { return m_loops; }

	AudioState Music::GetState() const {

		// wait for streaming thread to finish an iteration
		std::lock_guard lock(s_streaming_mutex);

		return GetRealTimeState();
	}
	AudioState Music::GetRealTimeState() const {
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
}