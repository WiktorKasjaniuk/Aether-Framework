
#pragma once

#include <Aether.hpp>
#include "GUIComponent.hpp"

class GUIButton : public GUIComponent {
public:
	GUIButton(const ae::Vector2f& size = ae::Vector2f(200.f, 60.f), const std::u32string& u32_string = U"Button", unsigned int char_size = 15);
	GUIButton(const GUIButton& copy);
	GUIButton(GUIButton&& move) noexcept;

	bool HandleEvent(const ae::EventMouseButtonPressed& event, const ae::Vector2f& button_position);
	void HandleEvent(const ae::EventMouseMoved& event, const ae::Vector2f& button_position);

	virtual void Draw(const ae::Matrix3x3& transform) const override;
	virtual ae::FloatRect GetBounds() const override;
	bool IsHovered(const ae::Vector2f& button_position) const;

	void SetSize(const ae::Vector2f& size);
	ae::Vector2f GetSize() const;

	void SetString(const std::u32string& u32_string);
	void SetCharSize(unsigned int char_size);
	void SetBold(unsigned char strength);

	void FixWidth(float padding);
	void FixHeight(float padding);


private:
	ae::Shape m_frame, m_content;
	ae::Text m_text;
};