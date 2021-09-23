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

#include "System/Events/EventCallbacks.hpp"
#include "System/Events/Event.hpp"
#include "System/Events/ContextEvent.hpp"
#include "System/Events/KeyboardEvent.hpp"
#include "System/Events/MouseEvent.hpp"
#include "System/Events/JoystickEvent.hpp"
#include "System/Events/TextEnteredEvent.hpp"

#include "Structure/LayerManager.hpp"
#include "Structure/Camera.hpp"

#include "System/Clock.hpp"

#include <glfw/glfw3.h>

#define AE_EVENTS_DOUBLE_CLICK_TIME_INTERVAL 0.3f
#define AE_EVENTS_DOUBLE_CLICK_OFFSET 15.f

namespace ae {
	namespace internal {
		void SubmitEvent(Event& event) {
			LayerManager.HandleEvent(event);
		}

		void SetEventCallbacks(void* native_window) {

			GLFWwindow* glfw_window = static_cast<GLFWwindow*>(native_window);

			glfwSetInputMode(glfw_window, GLFW_LOCK_KEY_MODS, GLFW_TRUE);

			// Callbacks
			glfwSetWindowCloseCallback(glfw_window, [](GLFWwindow* window) {
				glfwSetWindowShouldClose(window, GLFW_FALSE);

				ae::EventContextClosed event;
				SubmitEvent(event);
			});

			glfwSetWindowSizeCallback(glfw_window, [](GLFWwindow* window, int width, int height) {
				Camera.UpdateCameraSize(Vector2i(width, height));

				ae::EventContextResized event(Vector2i(width, height));
				SubmitEvent(event);
			});

			glfwSetWindowFocusCallback(glfw_window, [](GLFWwindow* window, int focused) {

				if (focused) {
					EventContextGainedFocus event;
					SubmitEvent(event);
				}
				else {
					EventContextLostFocus event;
					SubmitEvent(event);
				}

			});
			glfwSetKeyCallback(glfw_window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {

				KeyboardKey _key = static_cast<KeyboardKey>(key);
				KeyboardModifiers _mods = static_cast<KeyboardModifiers>(mods);

				switch (action) {

				case GLFW_RELEASE:
				{
					Input.Keyboard.SetKeyPressedState(KeyboardKeyState(_key, _mods), false);
					EventKeyReleased event(_key, _mods);
					SubmitEvent(event);
					break;
				}

				case GLFW_PRESS:
				{
					Input.Keyboard.SetKeyPressedState(KeyboardKeyState(_key, _mods), true);
					EventKeyPressed event(_key, _mods);
					SubmitEvent(event);
					break;
				}

				case GLFW_REPEAT:
				{
					EventKeyRepeated event(_key, _mods);
					SubmitEvent(event);
					break;
				}

				default:
					break;
				}

			});

			glfwSetMouseButtonCallback(glfw_window, [](GLFWwindow* window, int button, int action, int mods) {

				static MouseButton s_last_click_button;
				static Clock s_last_click_clock;
				static Vector2f s_last_click_position;

				MouseButton _button = static_cast<MouseButton>(button);
				KeyboardModifiers _mods = static_cast<KeyboardModifiers>(mods);

				bool _double = (
					s_last_click_button == _button && 
					s_last_click_clock.GetElapsedTime().GetSeconds() <= AE_EVENTS_DOUBLE_CLICK_TIME_INTERVAL &&
					fabs(Input.Mouse.GetPosition().x - s_last_click_position.x) < AE_EVENTS_DOUBLE_CLICK_OFFSET &&
					fabs(Input.Mouse.GetPosition().y - s_last_click_position.y) < AE_EVENTS_DOUBLE_CLICK_OFFSET
				);
				s_last_click_clock.Restart();
				s_last_click_button = _button;
				s_last_click_position = Input.Mouse.GetPosition();

				if (action == GLFW_PRESS)
				{
					Input.Mouse.SetButtonPressedState(MouseButtonState(_button, _mods), true);
					EventMouseButtonPressed event(_button, _mods, _double);
					SubmitEvent(event);
				}
				else
				{
					Input.Mouse.SetButtonPressedState(MouseButtonState(_button, _mods), false);
					EventMouseButtonReleased event(_button, _mods);
					SubmitEvent(event);
				}
			});

			glfwSetScrollCallback(glfw_window, [](GLFWwindow* window, double xoffset, double yoffset) {
				EventMouseScrolled event(Vector2f(static_cast<float>(xoffset), static_cast<float>(yoffset)));
				SubmitEvent(event);
			});

			glfwSetCursorPosCallback(glfw_window, [](GLFWwindow* window, double xpos, double ypos) {
				EventMouseMoved event(Vector2f(static_cast<float>(xpos), static_cast<float>(ypos)));
				SubmitEvent(event);
			});

			glfwSetCursorEnterCallback(glfw_window, [](GLFWwindow* window, int entered) {
				if (entered)
				{
					EventMouseEnteredContext event;
					SubmitEvent(event);
				}
				else
				{
					EventMouseLeftContext event;
					SubmitEvent(event);
				}
			});

			glfwSetJoystickCallback([](int jid, int event_type) {
				if (event_type == GLFW_CONNECTED) {
					EventJoystickConnected event(jid);
					SubmitEvent(event);
				}
				else {
					EventJoystickDisconnected event(jid);
					SubmitEvent(event);
				}
			});

			glfwSetCharCallback(glfw_window, [](GLFWwindow* window, unsigned int codepoint) {
				EventTextEntered event((char32_t)codepoint);
				SubmitEvent(event);
			});

		}


	} // namespace internal
}