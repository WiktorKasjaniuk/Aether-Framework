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

#include "Structure/LayerManager.hpp"

namespace ae {

	internal::LayerManagerType LayerManager = internal::CreateStructure<internal::LayerManagerType>();

	namespace internal {

		LayerManagerType::~LayerManagerType() {
			RemoveAllLayers();
		}

		Layer& LayerManagerType::GetLayer(size_t position) const {
			AE_ASSERT(GetSize() > position, "Layer at position '" << position << "' does not exist");
			return *m_layers.at(position);
		}

		size_t LayerManagerType::GetSize() const {
			return m_layers.size();
		}

		void LayerManagerType::RemoveLayer(bool on_top) {
			AE_ASSERT(GetSize() != 0, "No layers exist, therefore layers can be removed");

			if (on_top) {
				delete m_layers.back();
				m_layers.erase(m_layers.end() - 1);
			}
			else {
				delete m_layers.front();
				m_layers.erase(m_layers.begin());
			}
		}
		void LayerManagerType::RemoveLayer(size_t position) {
			AE_ASSERT(position < GetSize(), "Could not remove layer on position '" << position << "', vector out of range");

			delete m_layers.at(position);
			m_layers.erase(m_layers.begin() + position);
		}
		void LayerManagerType::RemoveLayer(Layer& layer) {
			auto layer_it = std::find(m_layers.begin(), m_layers.end(), &layer);
			AE_ASSERT(layer_it != m_layers.end(), "Could not remove layer, invalid layer");

			delete *layer_it;
			m_layers.erase(layer_it);
		}

		void LayerManagerType::RemoveAllLayers() {
			for (Layer* layer : m_layers)
				delete layer;

			m_layers.clear();
		}

		void LayerManagerType::BeginEventHandling() const {
			for (auto it(m_layers.rbegin()); it != m_layers.rend(); it++)
				(*it)->BeginEventHandling();
		}
		void LayerManagerType::EndEventHandling() const {
			for (auto it(m_layers.rbegin()); it != m_layers.rend(); it++)
				(*it)->EndEventHandling();
		}

		void LayerManagerType::HandleEvent(Event& event) const {
			for (auto it(m_layers.rbegin()); it != m_layers.rend(); it++) {
				(*it)->HandleEvent(event);

				if (event.IsHandled())
					break;
			}
		}
		void LayerManagerType::Draw() const {
			for (Layer* layer : m_layers)
				layer->Draw();
		}
		
	}
}