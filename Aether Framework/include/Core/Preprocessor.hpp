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

#include <iostream>
#include <sstream>

// macros for library compilation
#ifdef _DEBUG or NDEBUG
	#define AE_DEBUG
#endif

/// #define AE_DEVELOP

// debug mode for people using the library
#ifdef AE_DEBUG
	#define AE_DEBUG_ONLY(x) x

	#define AE_ASSERT(x, message) \
		if(!(x)) { \
			std::cout << "[Aether Error]\n" << message << "\n\n";\
			std::cin.get(); \
            std::exit(EXIT_FAILURE); \
		}

	#define AE_ASSERT_FALSE(message) \
		{ \
			std::cout << "[Aether Error]\n" << message << "\n\n";\
			std::cin.get(); \
            std::exit(EXIT_FAILURE); \
		}

	#define AE_WARNING(message) \
		std::cout << "[Aether Warning]\n" << message << "\n\n"

	#define AE_ASSERT_WARNING(x, message) \
		if(!(x)) { \
			AE_WARNING(message); \
		}		
		
#else
	#define AE_DEBUG_ONLY(x)
	#define AE_ASSERT(x, message)
	#define AE_ASSERT_FALSE(message)
	#define AE_WARNING(message)
	#define AE_ASSERT_WARNING(x, message)
#endif

// develop mode for people developing the library (me)
#ifdef AE_DEVELOP
	#define AE_GL_LOG(gl_operation)\
			AE_DEBUG_ONLY(ae::internal::ClearOpenGLErrors());\
			gl_operation;\
			AE_DEBUG_ONLY(ae::internal::LogOpenGLErrors(#gl_operation, __FILE__, __LINE__));

	#define AE_AL_LOG(al_operation)\
			AE_DEBUG_ONLY(ae::internal::ClearOpenALErrors());\
			al_operation;\
			AE_DEBUG_ONLY(ae::internal::LogOpenALErrors(#al_operation, __FILE__, __LINE__));

	#define AE_DEVELOP_ONLY(x) x

#else
	#define AE_GL_LOG(gl_operation) gl_operation
	#define AE_AL_LOG(al_operation) al_operation
	#define AE_DEVELOP_ONLY(x)

	#define STBI_ASSERT(x)

#endif