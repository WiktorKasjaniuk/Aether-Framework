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

#include "Event.hpp"
#include "../Vector2.hpp"
#include "../../Structure/Input.hpp"
#include "KeyboardModifiers.hpp"

namespace ae {

	class Event;

	namespace internal {
		class MouseButtonEvent : public Event {
		public:
			MouseButton GetButton() const { return m_button; }
			KeyboardModifiers GetModifiers() const { return m_modifiers; }

		protected:
			MouseButtonEvent(MouseButton button, KeyboardModifiers modifiers) : Event(), m_button(button), m_modifiers(modifiers) {}

		private:
			MouseButton m_button;
			KeyboardModifiers m_modifiers;
		};
	}

	class EventMouseButtonReleased : public internal::MouseButtonEvent {
		friend class ae::Event;

	public:
		EventMouseButtonReleased(MouseButton button, KeyboardModifiers modifiers) : internal::MouseButtonEvent(button, modifiers) {}

	protected:
		AE_EVENT_TYPE_FUNCTIONS(EventType::MouseButtonReleased);
	};

	class EventMouseButtonPressed : public internal::MouseButtonEvent {
		friend class ae::Event;

	public:
		EventMouseButtonPressed(MouseButton button, KeyboardModifiers modifiers, bool double_click) : internal::MouseButtonEvent(button, modifiers), m_double_click(double_click) {}
		bool IsDoubleClicked() const { return m_double_click; }

	protected:
		AE_EVENT_TYPE_FUNCTIONS(EventType::MouseButtonPressed);

	private:
		bool m_double_click;
	};

	class EventMouseScrolled : public Event {
		friend class ae::Event;

	public:
		EventMouseScrolled(const Vector2f& offset) : Event(), m_offset(offset) {}
		const Vector2f& GetOffset() const { return m_offset; }

	protected:
		AE_EVENT_TYPE_FUNCTIONS(EventType::MouseScrolled);

	private:
		Vector2f m_offset;
	};

	class EventMouseMoved : public Event {
		friend class ae::Event;

	public:
		EventMouseMoved(const Vector2f& position) : Event(), m_position(position) {}

		const Vector2f& GetPosition() const { return m_position; }

	protected:
		AE_EVENT_TYPE_FUNCTIONS(EventType::MouseMoved);

	private:
		Vector2f m_position;
	};

	class EventMouseEnteredContext : public Event {
		friend class ae::Event;

	public:
		EventMouseEnteredContext() : Event() {}

	protected:
		AE_EVENT_TYPE_FUNCTIONS(EventType::MouseEnteredContext);
	};

	class EventMouseLeftContext : public Event {
		friend class ae::Event;

	public:
		EventMouseLeftContext() : Event() {}

	protected:
		AE_EVENT_TYPE_FUNCTIONS(EventType::MouseLeftContext);
	};
}