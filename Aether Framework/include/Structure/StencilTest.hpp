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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// StencilTest
///
/// It will always be 8 stencil bits per fragment.
/// If the currently bound framebuffer has stencil testing enabled, 
/// the stencil test can be run to discard several fragments / pixels before being drawn.
/// To enable stencil testing in the default framebuffer, use ContextSettings for that purpose.
/// And remember, once fragments where drawn, they do not update with SetAction()!
///
/// Begin() and End()
///		are used to announce the start and the end of a stencil test.
/// 
/// SetBitModification(modifiable_bits)
///		is used to decide which bits can be modified during testing.
/// 
/// SetAction(StencilAction failed, StencilAction passed)
///		is used to set the action of failing and passing a test,
///		possible actions:
///			SetZero       - sets stencil buffer value to 0,
///			Keep          - keeps the value as it is,
///			Replace       - replaces the value with reference value set by calling SetPassCondition(condition, reference, and_bitmask),
///			Increment     - increments the value if it is lower than 255,
///			IncrementWrap - increments value and wraps it back to 0 if 255 is exceeded,
///			Decrement     - decrements the value if it is higher than 0,
///			DecrementWrap - decrements value and wraps it 255 if 0 is exceeded,
///			Invert        - inverts all bits.
/// 
/// SetPassCondition(StencilCondition condition, unsigned char reference, unsigned char and_bitmask)
///		is used to determine whenever the fragment should be discarded.
/// 
///		If condition is set to Always - it always passes, if set to Never - it never passes,
/// 
///		otherwise this formula is used:
///		should_pass = ( reference & and_bitmask ) condition ( stencil_value & and_bitmask ),
/// 
///		therefore if and_bitmask == 255 (1111 1111), it can be shortened to:
///		should_pass = ( reference ) condition ( stencil_value ),
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include "../Core/CreateStructure.hpp"

#include <cstdint>

namespace ae {

	enum class StencilAction {
		SetZero       = 0x0000, // GL_ZERO
		Keep          = 0x1E00, // GL_KEEP
		Replace       = 0x1E01, // GL_REPLACE
		Increment     = 0x1E02, // GL_INCR
		IncrementWrap = 0x8507, // GL_INCR_WRAP
		Decrement     = 0x1E03, // GL_DECR
		DecrementWrap = 0x8508, // GL_DECR_WRAP
		Invert        = 0x150A  // GL_INVERT
	};

	enum class StencilCondition {
		Always       = 0x0207, // GL_ALWAYS
		Never        = 0x0200, // GL_NEVER
		Equal        = 0x0202, // GL_EQUAL
		NotEqual     = 0x0205, // GL_NOTEQUAL
		Less         = 0x0201, // GL_LESS
		LessEqual    = 0x0203, // GL_LEQUAL
		Greater      = 0x0204, // GL_GREATER
		GreaterEqual = 0x0206  // GL_GEQUAL
	};

	namespace internal {
		class StencilTestType {
			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<StencilTestType>();

		public:
			void Begin();
			void End();

			void SetAction(StencilAction failed, StencilAction passed);
			void SetPassCondition(StencilCondition condition, std::uint8_t reference, std::uint8_t and_bitmask = 255);
			void SetBitModification(std::uint8_t modifiable_bits);

		private:
			StencilTestType() = default;
			StencilTestType(const StencilTestType&) = delete;
			StencilTestType(StencilTestType&&) = delete;
		};
	}

	extern ae::internal::StencilTestType StencilTest;
}