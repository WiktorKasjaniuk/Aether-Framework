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

#include "AudioSource.hpp"
#include "AudioDevice.hpp"

#include <string>
#include <vector>
#include <thread>
#include <future>
#include <mutex>

constexpr size_t c_music_buffer_count = 6;

namespace ae {
	namespace internal { class ApplicationType; }
	class Music;

	bool operator==(const Music& left, const Music& right);
	bool operator!=(const Music& left, const Music& right);

	class Music : public internal::AudioSource {
		friend class internal::ApplicationType;
		friend class internal::AudioDevice;

		friend bool operator==(const Music& left, const Music& right);
		friend bool operator!=(const Music& left, const Music& right);

	public:
		Music();
		Music(const Music& copy);
		Music(Music&& move) noexcept = delete;
		~Music();

		void SetFile(const std::string& filename);
		const std::string& GetFilename() const;

		virtual void Play() override;
		virtual void Stop() override;
		virtual void Pause() override;
		virtual void Resume() override;

		virtual AudioState GetState() const override;

		void SetLoop(bool should_loop);
		bool IsLooped() const;

	private:
		std::uint32_t m_sound_buffer_ids[c_music_buffer_count] = {};

		AudioState GetRealTimeState() const;

		// music data
		std::int32_t m_sample_rate = 0;
		std::uint8_t m_channel_count = 0;
		std::int32_t m_format = 0;

		// stream data
		std::string m_filename;
		void* m_native_file = nullptr;
		std::int16_t* m_stream_samples = nullptr;
		bool m_loops = false;

		void RequeueProcessedBuffer();
		void UpdateStream();
		void DiscardOldData();

		void RewindStreaming();
		void StopStreaming();

		static std::thread s_streaming_thread;
		static std::mutex s_streaming_mutex;
		static std::vector<Music*> s_streaming_tracks;
		static void UpdateTracks();
	};
}

