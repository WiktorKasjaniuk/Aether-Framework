
#pragma once

#include <Aether.hpp>
#include "GUIComponent.hpp"

class GUIChooseBox : public GUIComponent {
public:
	enum class Shape : bool {
		Circle, Square
	};

	GUIChooseBox(Shape shape = Shape::Square, const std::string& family = std::string());
	GUIChooseBox(const GUIChooseBox& copy);
	GUIChooseBox(GUIChooseBox&& move) noexcept;
	~GUIChooseBox();
	
	virtual void Draw(const ae::Matrix3x3& transform) const override;
	virtual ae::FloatRect GetBounds() const override;

	bool HandleEvent(const ae::EventMouseButtonPressed& event, const ae::Vector2f& box_position);
	void HandleEvent(const ae::EventMouseMoved& event, const ae::Vector2f& box_position);
	bool IsHovered(const ae::Vector2f& box_position) const;

	void SetFamily(const std::string& family);
	const std::string& GetFamily() const;
	void RemoveFromFamily();

	void SetShape(Shape shape);
	Shape GetShape() const;

	void SetString(const std::u32string& u32_string);
	void SetCharSize(unsigned int char_size);
	void SetBold(unsigned char strength);

	void SetChecked(bool check);
	bool IsChecked() const;

	void AllowManualUncheck(bool allow);

private:
	static std::unique_ptr<ae::Shape> s_frame_circle, s_content_circle, s_hovered_content_circle, s_check_circle;

	static std::unique_ptr<ae::Shape> s_frame_square, s_content_square, s_hovered_content_square;
	static std::unique_ptr<ae::Sprite> s_check_square;
	static std::unique_ptr<ae::Texture> s_checked_texture;

	ae::Text m_text;
	Shape m_shape;

	bool m_checked = false;
	bool m_hovered = false;
	bool m_allow_manual_uncheck = true;

	std::string m_family;
	
	static std::map<std::string, std::vector<GUIChooseBox*>> s_families;
};