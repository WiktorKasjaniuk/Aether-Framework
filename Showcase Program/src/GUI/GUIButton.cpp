
#include "GUI.hpp"

constexpr float c_border = 1.f;

GUIButton::GUIButton(const ae::Vector2f& size, const std::u32string& u32_string, unsigned int char_size) {
	m_frame.SetIndices({ 0,1,2,2,3,0 });
	m_frame.SetColor(GUI_COLOR_FRAME);

	m_content.SetIndices({ 0,1,2,2,3,0 });
	m_content.SetColor(GUI_COLOR_CONTENT);

	SetSize(size);
	SetString(u32_string);
	SetCharSize(char_size);

	m_text.SetFont(GUI_FONT);
	m_text.SetColor(GUI_COLOR_TEXT);
}
GUIButton::GUIButton(const GUIButton& copy)
	: m_frame(copy.m_frame), m_content(copy.m_content), m_text(copy.m_text) {}
GUIButton::GUIButton(GUIButton&& move) noexcept
	: m_frame(std::move(move.m_frame)), m_content(std::move(move.m_content)), m_text(std::move(move.m_text)) {}

bool GUIButton::HandleEvent(const ae::EventMouseButtonPressed& event, const ae::Vector2f& button_position) {

	if (event.GetButton() == ae::MouseButton::Left)
		return IsHovered(button_position);

	return false;
}
void GUIButton::HandleEvent(const ae::EventMouseMoved& event, const ae::Vector2f& button_position) {
	if (IsHovered(button_position))
		m_content.SetColor(GUI_COLOR_CONTENT_HOVERED);
	else
		m_content.SetColor(GUI_COLOR_CONTENT);
}

void GUIButton::Draw(const ae::Matrix3x3& transform) const {
	m_frame.Draw(transform);
	m_content.Draw(transform);

	const ae::FloatRect& text_bounds = m_text.GetBounds();
	float x_offset = std::floor((GetSize().x - text_bounds.width) / 2.f);
	float y_offset = std::floor((GetSize().y + text_bounds.height) / 2.f);

	ae::Matrix3x3 text_transform = transform.GetTranslated(ae::Vector2f(x_offset, y_offset));
	m_text.Draw(std::move(text_transform));
}

void GUIButton::SetSize(const ae::Vector2f& size) {

	ae::Vector2f f_size(std::floor(size.x), std::floor(size.y));

	std::vector<ae::VertexPos> vertices;

	vertices = {
		ae::Vector2f(0.f, 0.f), ae::Vector2f(f_size.x, 0.f),
		ae::Vector2f(f_size.x, f_size.y), ae::Vector2f(0.f, f_size.y)
	};

	m_frame.SetVertices(std::move(vertices));

	vertices = {
		ae::Vector2f(c_border, c_border), ae::Vector2f(f_size.x - c_border, c_border),
		ae::Vector2f(f_size.x - c_border, f_size.y - c_border), ae::Vector2f(c_border, f_size.y - c_border)
	};

	m_content.SetVertices(std::move(vertices));
}
ae::Vector2f GUIButton::GetSize() const { return m_frame.GetVertices()[2].position; }

void GUIButton::FixWidth(float padding) {
	ae::FloatRect text_bounds = m_text.GetBounds();
	SetSize(ae::Vector2f(text_bounds.width + 2.f * padding, GetSize().y));
}
void GUIButton::FixHeight(float padding) {
	ae::FloatRect text_bounds = m_text.GetBounds();
	SetSize(ae::Vector2f(GetSize().x, text_bounds.height + 2.f * padding));
}

ae::FloatRect GUIButton::GetBounds() const {

	const ae::FloatRect& text_bounds = m_text.GetBounds();
	float x_offset = std::min(0.f, std::floor((GetSize().x - text_bounds.width) / 2.f));
	float y_offset = std::min(0.f, std::floor((GetSize().y + text_bounds.height) / 2.f));

	return ae::FloatRect(
		ae::Vector2f(0.f, 0.f),
		GetSize()
	);
}

void GUIButton::SetString(const std::u32string& u32_string) { m_text.SetString(u32_string); }
void GUIButton::SetCharSize(unsigned int char_size) { m_text.SetCharSize(char_size); }
void GUIButton::SetBold(unsigned char strength) { m_text.SetBold(strength); }

bool GUIButton::IsHovered(const ae::Vector2f& button_position) const {
	ae::Vector2f mouse = ae::Input.Mouse.GetPosition();
	return (
		mouse.x >= button_position.x && mouse.y >= button_position.y &&
		mouse.x < button_position.x + GetSize().x && mouse.y < button_position.y + GetSize().y
	);
}