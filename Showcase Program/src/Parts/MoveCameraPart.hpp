
#pragma once
#include <Aether.hpp>
#include "Part.hpp"

class MoveCameraPart : public Part {
public:
	MoveCameraPart() {
		m_text.SetCharSize(50);
		m_text.SetFont(ae::AssetManager.GetFont("urbanist"));
		std::u32string string = U"Start by simply moving the camera       #\nusing keyboard keys\n(left bottom corner for bindings)\n\n#";
		string[string.find(U'#')] = 0x2192;
		string[string.rfind(U'#')] = 0x2193;
		m_text.SetString(std::move(string));
	}

	virtual void Draw(const ae::Matrix3x3& transform) override {
		m_text.Draw(transform);
	}

private:
	ae::Text m_text;
};