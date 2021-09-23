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


#include "Audio/AudioDevice.hpp"
#include "Audio/Listener.hpp"
#include "Audio/Music.hpp"

#include "Core/OpenALCalls.hpp"
#include "Core/Preprocessor.hpp"

#include "System/LogError.hpp"

#include <OpenALSoft/al.h>
#include <OpenALSoft/alc.h>

namespace ae {
	namespace internal {
		AudioDevice AudioDevice::s_instance;

		void AudioDevice::Initialize()
		{
			// open default device
			ALCdevice* device = alcOpenDevice(nullptr);
			ALCcontext* context = nullptr;

			// the application will be muted if no device are present at this point
			// or potentially crash if the default one is turned off after creation
			if (!device) {
				AE_WARNING("[Aether] Could not open OpenAL Soft device");
			}
			else {
				context = alcCreateContext(device, nullptr);
				bool context_current = alcMakeContextCurrent(context);
				AE_ASSERT_WARNING(context_current, "[Aether] Could not create and make current OpenAL Soft context");
			}

			// initialize listener
			Listener.Initialize();

			// update
			s_instance.m_native_device = static_cast<void*>(device);
			s_instance.m_native_context = static_cast<void*>(context);
		}

		void AudioDevice::Terminate() {
			s_instance.m_is_running = false;
			Music::s_streaming_thread.join();

			alcMakeContextCurrent(NULL);
			alcDestroyContext(static_cast<ALCcontext*>(s_instance.m_native_context));
			alcCloseDevice(static_cast<ALCdevice*>(s_instance.m_native_device));
		}

		bool AudioDevice::IsRunning() { return s_instance.m_is_running; }
	}
}