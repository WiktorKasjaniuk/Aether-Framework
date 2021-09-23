
#pragma once

#include <Aether.hpp>
#include "Part.hpp"

class StencilPart : public Part {
public:
	StencilPart();

	virtual void Draw(const ae::Matrix3x3& transform) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual void HandleEvent(ae::EventMouseButtonReleased& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;

private:
	ae::Shape m_sheets[3];
	ae::Vector2f m_sheet_offsets[3];
	int m_grabbed_sheet = -1;
	ae::Text m_description;
};