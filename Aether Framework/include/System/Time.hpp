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

#include <stdint.h>

namespace ae {

	class Time {
	public:
		Time();
		explicit Time(double seconds);
		explicit Time(int32_t milliseconds);
		explicit Time(int64_t microseconds);
		
		void SetSeconds(double seconds);
		void SetMilliseconds(int32_t milliseconds);
		void SetMicroseconds(int64_t microseconds);

		double GetSeconds() const;
		int32_t GetMilliseconds() const;
		int64_t GetMicroseconds() const;

		// Addition
		Time operator+(const Time& other) const;
		Time& operator+=(const Time& other);

		// Subtraction and Negation
		Time operator-(const Time& other) const;
		Time& operator-=(const Time& other);

		Time operator-() const;

		// Multiplication
		Time operator*(double other) const;
		Time& operator*=(double other);

		// Division
		Time operator/(double other) const;
		Time& operator/=(double other);

		// Comparation
		bool operator==(const Time& other) const;
		bool operator!=(const Time& other) const;

		bool operator<(const Time& other) const;
		bool operator<=(const Time& other) const;
		bool operator>(const Time& other) const;
		bool operator>=(const Time& other) const;

	private:
		int64_t m_microseconds;
	};
}