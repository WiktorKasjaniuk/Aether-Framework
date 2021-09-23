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

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Utf
///
/// Convertions:
///		Convert functions convert first parameter and replace the second with new value,
///		if something goes wrong, second parameter is replaced with fixed / working value.
/// 
///		Convert functions that return bool, return false if the string had to be fixed or cut.
/// 
///		
/// 
/// Validation:
///		IsValidUtf8() checks whether string is valid,
///		FixInvalidUtf8() fixes invalid utf-8 string (check Convert()).
/// 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#pragma once

#include <string>

namespace ae {
	namespace utf {
		bool Convert(const std::string& u8, std::u16string& u16);
		bool Convert(const std::string& u8, std::u32string& u32);
		bool Convert(const std::u16string& u16, std::string& u8);
		bool Convert(const std::u32string& u32, std::string& u8);

		std::string ConvertToUtf8(const std::u16string& u16);
		std::string ConvertToUtf8(const std::u32string& u32);
		std::u16string ConvertToUtf16(const std::string& u8);
		std::u32string ConvertToUtf32(const std::string& u8);
		
		bool IsValidUtf8(const std::string& u8string);
		std::string FixInvalidUtf8(const std::string& u8string);
	}
}