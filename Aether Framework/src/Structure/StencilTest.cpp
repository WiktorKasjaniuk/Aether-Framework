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

#include "Structure/StencilTest.hpp"

#include "Core/Preprocessor.hpp"
#include "Core/OpenGLCalls.hpp"

#include <glad/glad.h>

namespace ae {

	internal::StencilTestType StencilTest = internal::CreateStructure<internal::StencilTestType>();

	namespace internal {

		void StencilTestType::Begin() {
			AE_DEBUG_ONLY(
				int bits = 0;
				int object = 0;

				glGetIntegerv(GL_FRAMEBUFFER_BINDING, &object);

				if (object)
					glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &bits);
				else
					glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_STENCIL, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE, &bits);

				AE_ASSERT_WARNING(bits, "Currently bound Framebuffer does not support stencil testing");
			);
			
			AE_GL_LOG(glEnable(GL_STENCIL_TEST));
		}
		void StencilTestType::End() {
			AE_GL_LOG(glDisable(GL_STENCIL_TEST));
		}

		void StencilTestType::SetAction(StencilAction failed, StencilAction passed) {
			AE_GL_LOG(glStencilOp(
				static_cast<GLenum>(failed), 
				static_cast<GLenum>(passed), 
				static_cast<GLenum>(passed)
			));
		}
		void StencilTestType::SetPassCondition(StencilCondition condition, std::uint8_t reference, std::uint8_t and_bitmask) {
			AE_GL_LOG(glStencilFunc(static_cast<GLenum>(condition), static_cast<GLint>(reference), static_cast<GLuint>(and_bitmask)));
		}
		void StencilTestType::SetBitModification(std::uint8_t modifiable_bits) {
			AE_GL_LOG(glStencilMask(static_cast<GLuint>(modifiable_bits)));
		}
	}
}