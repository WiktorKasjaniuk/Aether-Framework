
#pragma once

#include <Aether.hpp>
#include "GUIComponent.hpp"

class GUISlider : public GUIComponent {
public:
	GUISlider(const ae::Vector2f& size = ae::Vector2f(140.f, 20.f));
	GUISlider(const GUISlider& copy);
	GUISlider(GUISlider&& move) noexcept;

	virtual void Draw(const ae::Matrix3x3& transform) const override;
	virtual ae::FloatRect GetBounds() const override;

	bool HandleEvent(const ae::EventMouseButtonPressed& event, const ae::Vector2f& slider_position);
	void HandleEvent(const ae::EventMouseButtonReleased& event, const ae::Vector2f& slider_position);
	void HandleEvent(const ae::EventMouseMoved& event, const ae::Vector2f& slider_position);
	bool IsHovered(const ae::Vector2f& slider_position) const;

	void SetString(const std::u32string& u32_string);
	const std::u32string& GetString() const;
	void SetCharSize(unsigned int char_size);
	void SetBold(unsigned char strength);

	void SetSize(const ae::Vector2f& size);
	ae::Vector2f GetSize() const;

	void SetValue(float value);
	float GetValue() const;

private:
	ae::Shape m_frame, m_content, m_thumb;

	float m_value = 0.f;
	bool m_grabbed = false;
	ae::Text m_text;
};