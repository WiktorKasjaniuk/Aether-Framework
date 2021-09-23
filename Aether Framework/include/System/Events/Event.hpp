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

///////////////////////////////////////////////////////////////////////////////////////////////////
/// Event System
///
/// When an event is generated it is passed to the LayerManager,
/// which gives the programmer to decide in which layer the event should be handled.
/// 
/// Dispatching:
///		Dispatch(Func<EventType> foo) is used to handle events,
///		foo is a templated funtion, 
///			- that takes specific event as a parameter 
///			- and return true if event was handled.
///		
///		When an event is dispatched, 
///			Dispatch(Func<EventType> foo) compares it's type with foo's type,
///			and if they are identical, foo is called.
/// 
///////////////////////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <functional>

#define AE_EVENT_TYPE_FUNCTIONS(event_type)\
	virtual EventType GetType() const override {\
		return event_type;\
	}\
	static EventType GetStaticType() {\
		return event_type;\
	}

namespace ae {

	enum class EventType {
		None,

		WindowClosed,
		WindowResized,
		WindowLostFocus,
		WindowGainedFocus,

		TextEntered,
		KeyPressed,
		KeyRepeated,
		KeyReleased,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseScrolled,
		MouseMoved,
		MouseEnteredContext,
		MouseLeftContext,

		JoystickConnected,
		JoystickDisconnected
	};

	class Event {
		template <typename EventType>
		using EventFunction = std::function<bool(EventType&)>;

	public:
		Event() : m_handled(false) {};
		virtual ~Event() {}

		template <typename EventType>
		void Dispatch(EventFunction<EventType> function) {

			if (GetType() == EventType::GetStaticType())
				m_handled = function( *((EventType*)this) );
		}

		bool IsHandled() const {
			return m_handled;
		}
		
	protected:
		virtual EventType GetType() const {
			return EventType::None;
		}

		static EventType GetStaticType() {
			return EventType::None;
		}

	private:
		bool m_handled;
	};

}