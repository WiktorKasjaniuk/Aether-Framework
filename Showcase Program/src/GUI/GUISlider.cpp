
#include "GUI.hpp"

constexpr float c_border = 1.f, c_thumb_width = 10.f, c_text_padding = 5.f;

GUISlider::GUISlider(const ae::Vector2f& size)
	: GUIComponent()
{
	m_text.SetColor(GUI_COLOR_TEXT);
	m_text.SetFont(GUI_FONT);
	m_text.SetCharSize(15);

	// shapes
	std::vector<unsigned int> indices = { 0,1,2, 2,3,0 };

	m_frame.SetColor(GUI_COLOR_FRAME);
	m_frame.SetIndices(indices);

	m_content.SetColor(GUI_COLOR_CONTENT);
	m_content.SetIndices(indices);
	
	m_thumb.SetColor(GUI_COLOR_FRAME);
	m_thumb.SetIndices(std::move(indices));

	SetSize(size);
}
GUISlider::GUISlider(const GUISlider& copy)
	: 
	m_frame(copy.m_frame), m_content(copy.m_content), m_thumb(copy.m_thumb),
	m_value(copy.m_value), m_text(copy.m_text) {}

GUISlider::GUISlider(GUISlider&& move) noexcept
	: 
	m_frame(std::move(move.m_frame)), m_content(std::move(move.m_content)), m_thumb(std::move(move.m_thumb)),
	m_value(std::move(move.m_value)), m_text(std::move(move.m_text)) {}

void GUISlider::Draw(const ae::Matrix3x3& transform) const {
	m_frame.Draw(transform);
	m_content.Draw(transform);

	float thumb_offset = std::floor(m_value * (GetSize().x - c_thumb_width));
	m_thumb.Draw(transform.GetTranslated(ae::Vector2f(thumb_offset, 0.f)));

	// text
	const ae::FloatRect& bounds = m_text.GetBounds();
	ae::Vector2f text_offset(
		GetSize().x + c_text_padding,
		std::floor((GetSize().y - bounds.top) / 2.f)

	);
	m_text.Draw(transform.GetTranslated(text_offset));
}
ae::FloatRect GUISlider::GetBounds() const {
	const ae::FloatRect& bounds = m_text.GetBounds();

	return ae::FloatRect(
		0.f, 0.f,

		GetSize().x
		+ (m_text.GetString().size() == 0 ? 0.f : (c_text_padding + bounds.left + bounds.width)),

		std::max(
			GetSize().y,
			std::floor((GetSize().y + bounds.top) / 2.f) + bounds.height
		)
	);
}

bool GUISlider::HandleEvent(const ae::EventMouseButtonPressed& event, const ae::Vector2f& slider_position) {
	if (IsHovered(slider_position) && event.GetButton() == ae::MouseButton::Left) {
		float value = ae::Input.Mouse.GetPosition().x - std::floor(c_thumb_width / 2.f) - slider_position.x;
		value /= (GetSize().x - c_thumb_width);
		value = std::max(std::min(value, 1.f), 0.f);
		SetValue(value);
		m_grabbed = true;
		return true;
	}
	return false;
}
void GUISlider::HandleEvent(const ae::EventMouseButtonReleased& event, const ae::Vector2f& slider_position) {
	if (event.GetButton() == ae::MouseButton::Left) {

		if (m_grabbed && !IsHovered(slider_position))
			m_content.SetColor(GUI_COLOR_CONTENT);

		m_grabbed = false;
	}
}
void GUISlider::HandleEvent(const ae::EventMouseMoved& event, const ae::Vector2f& slider_position) {

	if (IsHovered(slider_position) || m_grabbed)
		m_content.SetColor(GUI_COLOR_CONTENT_HOVERED);
	else
		m_content.SetColor(GUI_COLOR_CONTENT);

	if (m_grabbed) {
		float value = ae::Input.Mouse.GetPosition().x - std::floor(c_thumb_width / 2.f) - slider_position.x;
		value /= (GetSize().x - c_thumb_width);
		value = std::max(std::min(value, 1.f), 0.f);
		SetValue(value);
	}
}
bool GUISlider::IsHovered(const ae::Vector2f& slider_position) const {
	ae::Vector2f mouse = ae::Input.Mouse.GetPosition();

	return (
		mouse.x >= slider_position.x && mouse.y >= slider_position.y &&
		mouse.x < slider_position.x + GetSize().x && mouse.y < slider_position.y + GetSize().y
	);
}

void GUISlider::SetString(const std::u32string& u32_string) {
	m_text.SetString(u32_string);
}
const std::u32string& GUISlider::GetString() const {
	return m_text.GetString();
}
void GUISlider::SetCharSize(unsigned int char_size) {
	m_text.SetCharSize(char_size);
}
void GUISlider::SetBold(unsigned char strength) {
	m_text.SetBold(strength);
}

void GUISlider::SetSize(const ae::Vector2f& size) {

	m_frame.SetVertices(
		std::vector<ae::VertexPos> {
			ae::VertexPos(0.f, 0.f), ae::VertexPos(size.x, 0.f),
			ae::VertexPos(size.x, size.y), ae::VertexPos(0.f, size.y)
		}
	);

	m_content.SetVertices(
		std::vector<ae::VertexPos> {
			ae::VertexPos(c_border, c_border), ae::VertexPos(size.x - c_border, c_border),
			ae::VertexPos(size.x - c_border, size.y - c_border), ae::VertexPos(c_border, size.y - c_border)
		}
	);

	m_thumb.SetVertices(
		std::vector<ae::VertexPos> {
			ae::VertexPos(0.f, 0.f), ae::VertexPos(c_thumb_width, 0.f),
			ae::VertexPos(c_thumb_width, size.y), ae::VertexPos(0.f, size.y)
		}
	);
}
ae::Vector2f GUISlider::GetSize() const { 
	return m_frame.GetVertices()[2].position;
}

void GUISlider::SetValue(float value) { m_value = value; }
float GUISlider::GetValue() const { return m_value; }