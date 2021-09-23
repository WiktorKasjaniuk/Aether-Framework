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

// System
#include "System/Vector2.hpp"
#include "System/Vector3.hpp"
#include "System/Vector4.hpp"
#include "System/Rectangle.hpp"
#include "System/Time.hpp"
#include "System/Clock.hpp"
#include "System/utf.hpp"

#include "System/LogError.hpp"

#include "System/Events/Event.hpp"
#include "System/Events/ContextEvent.hpp"
#include "System/Events/KeyboardEvent.hpp"
#include "System/Events/MouseEvent.hpp"
#include "System/Events/JoystickEvent.hpp"
#include "System/Events/TextEnteredEvent.hpp"
#include "System/Events/EventCallbacks.hpp"

// Audio
#include "Audio/AudioDevice.hpp"
#include "Audio/SoundBuffer.hpp"
#include "Audio/AudioSource.hpp"
#include "Audio/Sound.hpp"
#include "Audio/Music.hpp"
#include "Audio/Listener.hpp"

// Graphics
#include "Graphics/Color.hpp"
#include "Graphics/Framebuffer.hpp"
#include "Graphics/Matrix3x3.hpp"
#include "Graphics/Transform2D.hpp"

#include "Graphics/Shader.hpp"
#include "Graphics/ShaderFunctions.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/TextureCanvas.hpp"
#include "Graphics/Font.hpp"

#include "Graphics/Rendering/VertexArray.hpp"
#include "Graphics/Rendering/Sprite.hpp"
#include "Graphics/Rendering/FrameSprite.hpp"
#include "Graphics/Rendering/Text.hpp"
#include "Graphics/Rendering/Shape.hpp"
#include "Graphics/Rendering/InstancedSpriteRenderer.hpp"
#include "Graphics/Rendering/BatchSpriteRenderer.hpp"

// Structure
#include "Structure/Application.hpp"
#include "Structure/SceneManager.hpp"
#include "Structure/AssetManager.hpp"
#include "Structure/Window.hpp"
#include "Structure/Camera.hpp"
#include "Structure/Input.hpp"
#include "Structure/LayerManager.hpp"
#include "Structure/Cursor.hpp"
#include "Structure/Clipboard.hpp"
#include "Structure/StencilTest.hpp"

#include "Structure/EntityComponentSystem/EntityManager.hpp"
#include "Structure/EntityComponentSystem/Utility.hpp"
#include "Structure/EntityComponentSystem/Entity.hpp"
#include "Structure/EntityComponentSystem/Component.hpp"
#include "Structure/EntityComponentSystem/ComponentPack.hpp"
#include "Structure/EntityComponentSystem/ViewTag.hpp"