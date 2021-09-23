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

#include "System/Time.hpp"

namespace ae {

	Time::Time() 
		: m_microseconds(0) {}

	Time::Time(double seconds)
		: m_microseconds(static_cast<int64_t>(seconds * 1000000.0)) {}

	Time::Time(int32_t milliseconds)
		: m_microseconds(static_cast<int64_t>(milliseconds) * 1000) {}

	Time::Time(int64_t microseconds)
		: m_microseconds(microseconds) {}


	void Time::SetSeconds(double seconds) {
		m_microseconds = static_cast<int64_t>(seconds * 1000000.0);
	}
	void Time::SetMilliseconds(int32_t milliseconds) {
		m_microseconds = static_cast<int64_t>(milliseconds) * 1000;
	}
	void Time::SetMicroseconds(int64_t microseconds) {
		m_microseconds = microseconds;
	}

	double Time::GetSeconds() const {
		return m_microseconds / 1000000.0;
	}
	int32_t Time::GetMilliseconds() const {
		return static_cast<int32_t>(m_microseconds / 1000);
	}
	int64_t Time::GetMicroseconds() const {
		return m_microseconds;
	}

	// Addition
	Time Time::operator+(const Time& other) const {
		return Time(m_microseconds + other.m_microseconds);
	}
	Time& Time::operator+=(const Time& other) {
		m_microseconds += other.m_microseconds;
		return *this;
	}

	// Subtraction and Negation
	Time Time::operator-(const Time& other) const {
		return Time(m_microseconds - other.m_microseconds);
	}
	Time& Time::operator-=(const Time& other) {
		m_microseconds -= other.m_microseconds;
		return *this;
	}

	Time Time::operator-() const {
		return Time(-m_microseconds);
	}

	// Multiplication
	Time Time::operator*(double other) const {
		return Time(static_cast<int64_t>(m_microseconds * other));
	}
	Time& Time::operator*=(double other) {
		m_microseconds = static_cast<int64_t>(m_microseconds * other);
		return *this;
	}

	// Division
	Time Time::operator/(double other) const {
		return Time(static_cast<int64_t>(m_microseconds / other));
	}
	Time& Time::operator/=(double other) {
		m_microseconds = static_cast<int64_t>(m_microseconds / other);
		return *this;
	}

	// Comparation
	bool Time::operator==(const Time& other) const {
		return (m_microseconds == other.m_microseconds);
	}
	bool Time::operator!=(const Time& other) const {
		return (m_microseconds != other.m_microseconds);
	}

	bool Time::operator<(const Time& other) const {
		return (m_microseconds < other.m_microseconds);
	}
	bool Time::operator<=(const Time& other) const {
		return (m_microseconds <= other.m_microseconds);
	}
	bool Time::operator>(const Time& other) const {
		return (m_microseconds > other.m_microseconds);
	}
	bool Time::operator>=(const Time& other) const {
		return (m_microseconds >= other.m_microseconds);
	}
}