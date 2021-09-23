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

namespace ae {
	enum class KeyboardModifiers : int {
		None = 0x0000,
		Shift = 0x0001,		// GLFW_MOD_SHIFT 
		Control = 0x0002,	// GLFW_MOD_CONTROL
		Alt = 0x0004,		// GLFW_MOD_ALT
		System = 0x0008,	// GLFW_MOD_SUPER
		CapsLock = 0x0010,	// GLFW_MOD_CAPS_LOCK
		NumLock = 0x0020	// GLFW_MOD_NUM_LOCK
	};

	inline KeyboardModifiers operator|(const KeyboardModifiers left, const KeyboardModifiers right) {
		return static_cast<KeyboardModifiers>(static_cast<int>(left) | static_cast<int>(right));
	}
	inline KeyboardModifiers operator&(const KeyboardModifiers left, const KeyboardModifiers right) {
		return static_cast<KeyboardModifiers>(static_cast<int>(left) & static_cast<int>(right));
	}
}