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

#include "../Core/CreateStructure.hpp"
#include "../System/Events/EventCallbacks.hpp"
#include "../System/Events/KeyboardModifiers.hpp"
#include "../System/Vector2.hpp"

#include <string>
#include <vector>

namespace ae {

	enum class MouseButton : int {
		Left,
		Right,
		Middle,
		Extra1,
		Extra2,
		Extra3,
		Extra4,
		Extra5
	};
	enum class KeyboardKey : int {
		Space = 32,
		Apostrophe = 39, // ' 
		Comma = 44, // , 
		Minus = 45, // - 
		Period = 46, // . 
		Slash = 47, // / 
		Num0 = 48,
		Num1 = 49,
		Num2 = 50,
		Num3 = 51,
		Num4 = 52,
		Num5 = 53,
		Num6 = 54,
		Num7 = 55,
		Num8 = 56,
		Num9 = 57,
		Semicolon = 59, // ; 
		Equal = 61, // = 
		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,
		LeftBracket = 91, // [ 
		Backslash = 92, // \ 
		RightBracket = 93, // ] 
		GraveAccent = 96, // ` 
		Tilde = 96, // ` 
		World1 = 161, // non-us #1 
		World2 = 162, // non-us #2 
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,
		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		LeftSystem = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		RightSystem = 347,
		Menu = 348
	};
	enum class JoystickHatState : int {
		Center = 0,

		Up = 1,
		Right = 1 << 1,
		Down = 1 << 2,
		Left = 1 << 3,

		RightUp = Right | Up,
		RightDown = Right | Down,
		LeftUp = Left | Up,
		LeftDown = Left | Down
	};

	class KeyboardKeyState;
	class MouseButtonState;

	namespace internal {

		class InputType;

		// Mouse
		class MouseType {
			friend class ae::internal::InputType;
			friend void SetEventCallbacks(void* native_window);

		public:
			void SetRawMontion(bool set);

			void SetPosition(const Vector2f& position, bool relative_to_window = true);
			Vector2f GetPosition(bool relative_to_window = true) const;

			bool IsButtonPressed(MouseButton button) const;
			bool IsAnyButtonPressed() const;
			const std::vector<MouseButtonState>& GetPressedButtonStates() const;
			bool IsInsideWindow() const;

			std::string GetButtonName(MouseButton button) const;

		private:
			MouseType() = default;
			MouseType(const MouseType&) = delete;
			MouseType(MouseType&&) = delete;

			std::vector<MouseButtonState> m_pressed_buttons;
			void SetButtonPressedState(MouseButtonState state, bool pressed);
		};

		// Keyboard
		class KeyboardType {
			friend class ae::internal::InputType;
			friend void SetEventCallbacks(void* native_window);

		public:
			bool IsKeyPressed(KeyboardKey key) const;
			bool IsAnyKeyPressed() const;
			const std::vector<KeyboardKeyState>& GetPressedKeyStates() const;
			std::string GetKeyName(KeyboardKey key) const;

		private:
			KeyboardType() = default;
			KeyboardType(const KeyboardType&) = delete;
			KeyboardType(KeyboardType&&) = delete;

			std::vector<KeyboardKeyState> m_pressed_keys;
			void SetKeyPressedState(KeyboardKeyState state, bool pressed);
		};

		// Joystick
		class JoystickType {
			friend class ae::internal::InputType;

		public:
			bool IsConnected(unsigned int joystick) const;
			
			unsigned int GetButtonCount(unsigned int joystick) const;
			bool IsButtonPressed(unsigned int joystick, unsigned int button) const;

			unsigned int GetAxisCount(unsigned int joystick) const;
			float GetAxisPosition(unsigned int joystick, unsigned int axis) const;
			
			unsigned int GetHatCount(unsigned int joystick) const;
			JoystickHatState GetHatState(unsigned int joystick, unsigned int hat) const;
			
			std::string GetName(unsigned int joystick) const;

		private:
			JoystickType() = default;
			JoystickType(const JoystickType&) = delete;
			JoystickType(JoystickType&&) = delete;
		};
		

		// Input
		class ApplicationType;

		class InputType {
			friend class ae::internal::ApplicationType;

			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<InputType>();

		public:
			MouseType Mouse;
			KeyboardType Keyboard;
			JoystickType Joystick;

		private:
			InputType();
			InputType(const InputType&) = delete;
			InputType(InputType&&) = delete;
		};
		
	}

	class KeyboardKeyState {
	public:
		KeyboardKeyState(KeyboardKey key, KeyboardModifiers modifiers);

		KeyboardKey GetKey() const;
		KeyboardModifiers GetModifiers() const;

		bool operator==(const KeyboardKeyState& other);

	private:
		KeyboardKey m_key;
		KeyboardModifiers m_modifiers;
	};
	class MouseButtonState {
	public:
		MouseButtonState(MouseButton button, KeyboardModifiers modifiers);

		MouseButton GetButton() const;
		KeyboardModifiers GetModifiers() const;

		bool operator==(const MouseButtonState& other);

	private:
		MouseButton m_button;
		KeyboardModifiers m_modifiers;
	};

	extern ae::internal::InputType Input;
}

