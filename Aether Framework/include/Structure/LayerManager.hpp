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

//////////////////////////////////////////////////////////////
/// Layers
///
/// General Idea:
///		Layer is an abstract class representing application's drawing and event-handling purposes.
/// 
///	Layer Queue:
///		Each layer is positioned in a queue from top to bottom, 
///		the order decides which layer get to be drawn or handle an event first.
/// 
/// Event Handling:
///		When an event is generated it travels from top layer to the bottom,
///		by dispatching the event, the programmer can decide if it is handled or should be passed to the next layer.
/// 
/// Drawing:
///		If the window is visible, layers are drawn in a queue from top to bottom.
///
//////////////////////////////////////////////////////////////
#pragma once

#include "../Core/Preprocessor.hpp"
#include "../Core/CreateStructure.hpp"
#include "../System/Events/Event.hpp"

#include <vector>

namespace ae {


	class Layer {
	public:
		Layer() {}
		virtual ~Layer() {}

		virtual void BeginEventHandling() {}
		virtual void EndEventHandling() {}

		virtual void HandleEvent(Event& event) = 0;
		virtual void Draw() = 0;
	};

	namespace internal {

		class ApplicationType;
		void SubmitEvent(Event& event);

		class LayerManagerType {
			friend class ae::internal::ApplicationType;
			friend void ae::internal::SubmitEvent(Event& event);

			template <typename SingletonType>
			friend SingletonType ae::internal::CreateStructure<LayerManagerType>();

		public:
			~LayerManagerType();

			template <typename LayerType, typename ...ArgsType>
			LayerType& CreateLayer(bool on_top, ArgsType&&... args) {
				LayerType* layer = new LayerType(std::forward<ArgsType>(args)...);

				if (on_top)
					m_layers.emplace_back(layer);
				else
					m_layers.emplace(m_layers.begin(), layer);

				return *layer;
			}

			template <typename LayerType, typename ...ArgsType>
			LayerType& CreateLayer(size_t position, ArgsType&&... args) {
				AE_ASSERT(position < GetSize(), "Could not create layer on position '" << position << "', vector out of range");

				LayerType* layer = new LayerType(std::forward<ArgsType>(args)...);
				m_layers.emplace(m_layers.begin() + position, layer);
				return *layer;
			}

			Layer& GetLayer(size_t position) const;
			size_t GetSize() const;

			void RemoveLayer(bool on_top = true);
			void RemoveLayer(size_t position);
			void RemoveLayer(Layer& layer);

			void RemoveAllLayers();

		private:
			std::vector<Layer*> m_layers; // layers should be operated in a queue, not accessed

			LayerManagerType() = default;
			LayerManagerType(const LayerManagerType&) = delete;
			LayerManagerType(LayerManagerType&&) = delete;

			void HandleEvent(Event& event) const;
			void BeginEventHandling() const;
			void EndEventHandling() const;
			void Draw() const;
		};
	}

	extern ae::internal::LayerManagerType LayerManager;

}