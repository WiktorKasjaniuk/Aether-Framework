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

#include "System/Utf.hpp"
#include <iostream>

#define UTF_CPP_CPLUSPLUS 201500 // ensure C++17
#include "utf8cpp/utf8.h"

namespace ae {
	namespace utf {

		// From UTF-8
		bool Convert(const std::string& u8, std::u16string& u16) {
			try {
				u16 = utf8::utf8to16(u8);
				return true;
			}
			catch (const utf8::exception&) {
				u16 = utf8::utf8to16(utf8::replace_invalid(u8));
				return false;
			}
		}
		bool Convert(const std::string& u8, std::u32string& u32) {
			try {
				u32 = utf8::utf8to32(u8);
				return true;
			}
			catch (const utf8::exception&) {
				u32 = utf8::utf8to32(utf8::replace_invalid(u8));
				return false;
			}
		}


		// From UTF-16
		bool Convert(const std::u16string& u16, std::string& u8) {
			u8.clear();

			try {
				utf8::utf16to8(u16.begin(), u16.end(), back_inserter(u8));
				return true;
			}
			catch (const utf8::exception&) {
				u8.append("\0");
				return false;
			}
		}


		// From UTF-32
		bool Convert(const std::u32string& u32, std::string& u8) {
			u8.clear();

			try {
				utf8::utf32to8(u32.begin(), u32.end(), back_inserter(u8));
				return true;
			}
			catch (const utf8::exception&) {
				u8.append("\0");
				return false;
			}
		}

		std::string ConvertToUtf8(const std::u16string& u16) {
			std::string u8;
			Convert(u16, u8);
			return std::move(u8);
		}
		std::string ConvertToUtf8(const std::u32string& u32) {
			std::string u8;
			Convert(u32, u8);
			return std::move(u8);
		}
		std::u16string ConvertToUtf16(const std::string& u8) {
			std::u16string u16;
			Convert(u8, u16);
			return std::move(u16);
		}
		std::u32string ConvertToUtf32(const std::string& u8) {
			std::u32string u32;
			Convert(u8, u32);
			return std::move(u32);
		}


		// Other
		bool IsValidUtf8(const std::string& u8string) {
			return (utf8::find_invalid(u8string) == u8string.npos);
		}
		std::string FixInvalidUtf8(const std::string& u8string) {
			return utf8::replace_invalid(u8string);
		}

	}
}