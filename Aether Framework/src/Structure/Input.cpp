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

#include "Structure/Input.hpp"
#include "Structure/Window.hpp"

#include <glfw/glfw3.h>
#include <iostream>

namespace ae {

	internal::InputType Input = internal::CreateStructure< internal::InputType>();

	namespace internal {

		InputType::InputType() {}
		
		void MouseType::SetPosition(const Vector2f& position, bool relative_to_window) {

			if (relative_to_window)
				glfwSetCursorPos(static_cast<GLFWwindow*>(GetNativeWindow()), position.x, position.y);

			else {
				Vector2i window_position = ae::Window.GetPosition();
				Vector2f new_position (
					position.x - static_cast<float>(window_position.x),
					position.y - static_cast<float>(window_position.y)
				);

				glfwSetCursorPos(static_cast<GLFWwindow*>(GetNativeWindow()), new_position.x, new_position.y);
			}
		}
		Vector2f MouseType::GetPosition(bool relative_to_window) const {
			Vector2<double> position;
			glfwGetCursorPos(static_cast<GLFWwindow*>(GetNativeWindow()), &position.x, &position.y);

			if (relative_to_window)
				return Vector2f(static_cast<float>(position.x), static_cast<float>(position.y));

			Vector2i window_position = ae::Window.GetPosition();

			return Vector2f(
				window_position.x + static_cast<float>(position.x),
				window_position.y + static_cast<float>(position.y)
			);
		}

		void MouseType::SetRawMontion(bool set) {
			if (glfwRawMouseMotionSupported())
				glfwSetInputMode(static_cast<GLFWwindow*>(GetNativeWindow()), GLFW_RAW_MOUSE_MOTION, set);
		}

		bool MouseType::IsButtonPressed(MouseButton button) const {
			return (glfwGetMouseButton(static_cast<GLFWwindow*>(GetNativeWindow()), static_cast<int>(button)) == GLFW_PRESS);
		}
		bool MouseType::IsAnyButtonPressed() const { return !m_pressed_buttons.empty(); }
		const std::vector<MouseButtonState>& MouseType::GetPressedButtonStates() const { return m_pressed_buttons; }
		bool MouseType::IsInsideWindow() const {
			return glfwGetWindowAttrib(static_cast<GLFWwindow*>(GetNativeWindow()), GLFW_HOVERED);

		}
		std::string MouseType::GetButtonName(MouseButton button) const {
			
			switch (button)
			{
			case MouseButton::Left:
				return "Left Mouse Button";

			case MouseButton::Right:
				return "Right Mouse Button";

			case MouseButton::Middle:
				return "Middle Mouse Button";

			case MouseButton::Extra1:
				return "Mouse Extra 1";

			case MouseButton::Extra2:
				return "Mouse Extra 2";

			case MouseButton::Extra3:
				return "Mouse Extra 3";

			case MouseButton::Extra4:
				return "Mouse Extra 4";

			case MouseButton::Extra5:
				return "Mouse Extra 5";

			default:
				return "Mouse Unknown";
			}
		}

		void MouseType::SetButtonPressedState(MouseButtonState state, bool pressed) {
			auto states_it = std::find(m_pressed_buttons.begin(), m_pressed_buttons.end(), state);

			if (pressed) {
				if (states_it == m_pressed_buttons.end())
					m_pressed_buttons.push_back(state);
			}
			else {
				if (states_it != m_pressed_buttons.end())
					m_pressed_buttons.erase(states_it);
			}
		}

		bool KeyboardType::IsKeyPressed(KeyboardKey key) const {
			return (glfwGetKey(static_cast<GLFWwindow*>(GetNativeWindow()), static_cast<int>(key)) == GLFW_PRESS);
		}
		bool KeyboardType::IsAnyKeyPressed() const { return !m_pressed_keys.empty(); }
		const std::vector<KeyboardKeyState>& KeyboardType::GetPressedKeyStates() const { return m_pressed_keys; }
			 
		std::string KeyboardType::GetKeyName(KeyboardKey key) const {
			const char* name = glfwGetKeyName(static_cast<int>(key), glfwGetKeyScancode(static_cast<int>(key)));

			if (name == NULL) 
			{
				switch (key)
				{
					case KeyboardKey::Backspace:	return "Backspace";
					case KeyboardKey::CapsLock:		return "CapsLock";
					case KeyboardKey::Delete:		return "Delete";
					case KeyboardKey::Down:			return "Down";
					case KeyboardKey::End:			return "End";
					case KeyboardKey::Enter:		return "Enter";
					case KeyboardKey::Escape:		return "Escape";
					case KeyboardKey::F1:			return "F1";
					case KeyboardKey::F2:			return "F2";
					case KeyboardKey::F3:			return "F3";
					case KeyboardKey::F4:			return "F4";
					case KeyboardKey::F5:			return "F5";
					case KeyboardKey::F6:			return "F6";
					case KeyboardKey::F7:			return "F7";
					case KeyboardKey::F8:			return "F8";
					case KeyboardKey::F9:			return "F9";
					case KeyboardKey::F10:			return "F10";
					case KeyboardKey::F11:			return "F11";
					case KeyboardKey::F12:			return "F12";
					case KeyboardKey::F13:			return "F13";
					case KeyboardKey::F14:			return "F14";
					case KeyboardKey::F15:			return "F15";
					case KeyboardKey::F16:			return "F16";
					case KeyboardKey::F17:			return "F17";
					case KeyboardKey::F18:			return "F18";
					case KeyboardKey::F19:			return "F19";
					case KeyboardKey::F20:			return "F20";
					case KeyboardKey::F21:			return "F21";
					case KeyboardKey::F22:			return "F22";
					case KeyboardKey::F23:			return "F23";
					case KeyboardKey::F24:			return "F24";
					case KeyboardKey::F25:			return "F25";
					case KeyboardKey::Home:			return "Home";
					case KeyboardKey::Insert:		return "Insert";
					case KeyboardKey::KP0:			return "KP0";
					case KeyboardKey::KP1:			return "KP1";
					case KeyboardKey::KP2:			return "KP2";
					case KeyboardKey::KP3:			return "KP3";
					case KeyboardKey::KP4:			return "KP4";
					case KeyboardKey::KP5:			return "KP5";
					case KeyboardKey::KP6:			return "KP6";
					case KeyboardKey::KP7:			return "KP7";
					case KeyboardKey::KP8:			return "KP8";
					case KeyboardKey::KP9:			return "KP9";
					case KeyboardKey::Left:			return "Left";
					case KeyboardKey::LeftAlt:		return "LeftAlt";
					case KeyboardKey::LeftControl:	return "LeftControl";
					case KeyboardKey::LeftShift:	return "LeftShift";
					case KeyboardKey::LeftSystem:	return "LeftSystem"; // LeftSuper
					case KeyboardKey::Menu:			return "Menu";
					case KeyboardKey::NumLock:		return "NumLock";
					case KeyboardKey::PageDown:		return "PageDown";
					case KeyboardKey::PageUp:		return "PageUp";
					case KeyboardKey::Pause:		return "Pause";
					case KeyboardKey::Period:		return "Period";
					case KeyboardKey::PrintScreen:	return "PrintScreen";
					case KeyboardKey::Right:		return "Right";
					case KeyboardKey::RightAlt:		return "RightAlt";
					case KeyboardKey::RightControl:	return "RightControl";
					case KeyboardKey::RightShift:	return "RightShift";
					case KeyboardKey::RightSystem:	return "RightSystem"; // RightSuper
					case KeyboardKey::ScrollLock:	return "ScrollLock";
					case KeyboardKey::Space:		return "Space";
					case KeyboardKey::Tab:			return "Tab";
				    case KeyboardKey::Tilde:		return "Tilde"; // GraveAccent
					case KeyboardKey::Up:			return "Up";

					default:
						return "Key Unknown";
				}

			}
			return name;
		}

		void KeyboardType::SetKeyPressedState(KeyboardKeyState state, bool pressed) {
			auto states_it = std::find(m_pressed_keys.begin(), m_pressed_keys.end(), state);

			if (pressed) {
				if (states_it == m_pressed_keys.end())
					m_pressed_keys.push_back(state);
			}
			else {
				if (states_it != m_pressed_keys.end())
					m_pressed_keys.erase(states_it);
			}
		}

		bool JoystickType::IsConnected(unsigned int joystick) const {
			return glfwJoystickPresent(joystick);
		}

		unsigned int JoystickType::GetButtonCount(unsigned int joystick) const {
			int count;
			glfwGetJoystickButtons(joystick, &count);
			return count;
		}
		bool JoystickType::IsButtonPressed(unsigned int joystick, unsigned int button) const {
			int count;
			const unsigned char* buttons = glfwGetJoystickButtons(joystick, &count);

			if (static_cast<unsigned int>(count) <= button)
				return false;

			return buttons[button];
		}

		unsigned int JoystickType::GetAxisCount(unsigned int joystick) const {
			int count;
			glfwGetJoystickAxes(joystick, &count);
			return count;
		}
		float JoystickType::GetAxisPosition(unsigned int joystick, unsigned int axis) const {
			int count;
			const float* axes = glfwGetJoystickAxes(joystick, &count);

			if (static_cast<unsigned int>(count) <= axis)
				return false;

			return axes[axis];
		}
		
		unsigned int JoystickType::GetHatCount(unsigned int joystick) const {
			int count;
			glfwGetJoystickHats(joystick, &count);
			return count;
		}
		JoystickHatState JoystickType::GetHatState(unsigned int joystick, unsigned int hat) const {
			int count;
			const unsigned char* hats = glfwGetJoystickHats(joystick, &count);

			if (static_cast<unsigned int>(count) <= hat)
				return JoystickHatState::Center;

			return static_cast<JoystickHatState>(hats[hat]);
		}
		std::string JoystickType::GetName(unsigned int joystick) const {
			return glfwGetJoystickName(joystick);
		}

	} // internal

	// caching key states
	KeyboardKeyState::KeyboardKeyState(KeyboardKey key, KeyboardModifiers modifiers)
		: m_key(key), m_modifiers(modifiers) {}
	KeyboardKey KeyboardKeyState::GetKey() const { return m_key; }
	KeyboardModifiers KeyboardKeyState::GetModifiers() const { return m_modifiers; }

	bool KeyboardKeyState::operator==(const KeyboardKeyState& other) {
		return (m_key == other.m_key);
	}

	// caching button states
	MouseButtonState::MouseButtonState(MouseButton button, KeyboardModifiers modifiers)
		: m_button(button), m_modifiers(modifiers) {}
	MouseButton MouseButtonState::GetButton() const { return m_button; }
	KeyboardModifiers MouseButtonState::GetModifiers() const { return m_modifiers; }

	bool MouseButtonState::operator==(const MouseButtonState& other) {
		return (m_button == other.m_button);
	}

} // ae