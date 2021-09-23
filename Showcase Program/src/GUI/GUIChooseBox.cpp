
#include "GUI.hpp"

constexpr int c_vertices_count = 16;
constexpr float c_pi = 3.14159265f, c_radius = 10.f, c_border = 1.f, c_eye_radius = 2.f, c_diameter = c_radius * 2.f, c_check_thickness = 2.f;

constexpr float c_text_padding = 5.f;

std::unique_ptr<ae::Shape> GUIChooseBox::s_frame_circle = nullptr, GUIChooseBox::s_content_circle = nullptr, GUIChooseBox::s_hovered_content_circle = nullptr, GUIChooseBox::s_check_circle = nullptr;
std::unique_ptr<ae::Shape> GUIChooseBox::s_frame_square = nullptr, GUIChooseBox::s_content_square = nullptr, GUIChooseBox::s_hovered_content_square = nullptr;
std::unique_ptr<ae::Sprite> GUIChooseBox::s_check_square = nullptr;
std::unique_ptr<ae::Texture> GUIChooseBox::s_checked_texture = nullptr;

std::map<std::string, std::vector<GUIChooseBox*>> GUIChooseBox::s_families;

GUIChooseBox::GUIChooseBox(Shape shape, const std::string& family) {

	if (!s_frame_circle)
	{
		/////////////////////////////////
		// circle
		/////////////////////////////////

		// calculate circle vertices
		std::vector<ae::Vector2f> raw_vertices;
		std::vector<unsigned int> indices;

		// middle one
		raw_vertices.emplace_back(0.f, 0.f);
		indices.push_back(0);

		// vertices
		for (int v(0); v < c_vertices_count; v++) {

			float angle = v * (2.f * c_pi) / c_vertices_count;

			raw_vertices.emplace_back(
				cos(angle),
				sin(angle)
			);
			indices.push_back(v + 1);
		}
		indices.push_back(1);

		// frame
		std::vector<ae::VertexPos> vertices;
		vertices.reserve(raw_vertices.size());

		for (const ae::Vector2f& vertex : raw_vertices)
			vertices.emplace_back(vertex * c_radius + ae::Vector2f(c_radius, c_radius));

		s_frame_circle.reset(new ae::Shape());
		s_frame_circle->SetVertices(std::move(vertices));
		s_frame_circle->SetIndices(indices);
		s_frame_circle->SetDrawMode(ae::DrawMode::TriangleFan);
		s_frame_circle->SetColor(GUI_COLOR_FRAME);

		// content
		vertices = {};
		vertices.reserve(raw_vertices.size());

		for (const ae::Vector2f& vertex : raw_vertices)
			vertices.emplace_back(vertex * (c_radius - c_border) + ae::Vector2f(c_radius, c_radius));

		s_content_circle.reset(new ae::Shape());
		s_content_circle->SetVertices(std::move(vertices));
		s_content_circle->SetIndices(indices);
		s_content_circle->SetDrawMode(ae::DrawMode::TriangleFan);
		s_content_circle->SetColor(GUI_COLOR_CONTENT);

		// hovered content
		s_hovered_content_circle.reset(new ae::Shape(*s_content_circle));
		s_hovered_content_circle->SetColor(GUI_COLOR_CONTENT_HOVERED);

		// check symbol
		vertices = {};
		vertices.reserve(raw_vertices.size());

		for (const ae::Vector2f& vertex : raw_vertices)
			vertices.emplace_back(vertex * c_eye_radius + ae::Vector2f(c_radius, c_radius));

		s_check_circle.reset(new ae::Shape());
		s_check_circle->SetVertices(std::move(vertices));
		s_check_circle->SetIndices(indices);
		s_check_circle->SetDrawMode(ae::DrawMode::TriangleFan);
		s_check_circle->SetColor(GUI_COLOR_FRAME);

		/////////////////////////////////
		// square
		/////////////////////////////////
		indices = { 0,1,2, 2,3,0 };

		// frame
		vertices = {
			ae::Vector2f(0.f, 0.f), ae::Vector2f(c_diameter, 0.f),
			ae::Vector2f(c_diameter, c_diameter), ae::Vector2f(0.f, c_diameter)
		};

		s_frame_square.reset(new ae::Shape());
		s_frame_square->SetVertices(std::move(vertices));
		s_frame_square->SetIndices(indices);
		s_frame_square->SetColor(GUI_COLOR_FRAME);

		// content
		vertices = {
			ae::Vector2f(c_border, c_border), ae::Vector2f(c_diameter - c_border, c_border),
			ae::Vector2f(c_diameter - c_border, c_diameter - c_border), ae::Vector2f(c_border, c_diameter - c_border)
		};

		s_content_square.reset(new ae::Shape());
		s_content_square->SetVertices(std::move(vertices));
		s_content_square->SetIndices(indices);
		s_content_square->SetColor(GUI_COLOR_CONTENT);

		// hovered content
		s_hovered_content_square.reset(new ae::Shape(*s_content_square));
		s_hovered_content_square->SetColor(GUI_COLOR_CONTENT_HOVERED);

		// check symbol
		s_checked_texture.reset(new ae::Texture());
		ae::TextureCanvas canvas;
		canvas.Create(ae::Vector2f(c_diameter, c_diameter), ae::Color::Transparent);
	
		for (int x(0); x < canvas.GetSize().x; x++)
			for (int y(0); y < canvas.GetSize().y; y++) {

				int _y = canvas.GetSize().y - y;
				float _y1 = -_y + c_diameter * 0.7f;
				float _y2 = 0.5f * _y + c_diameter * 0.3f;

				bool lower_than_cross = x < 0.44f * c_diameter;
				bool left_line = lower_than_cross && x > 0.2f * c_diameter;
				bool right_line = !lower_than_cross && x < 0.7f * c_diameter;

				if (
					(left_line && x >= _y1 - 0.5f * c_check_thickness && x < _y1 + 0.5f * c_check_thickness) ||
					(right_line && x >= _y2 - 0.5f * c_check_thickness && x < _y2 + 0.5f * c_check_thickness)
				)

				canvas.Px(x, y) = ae::Color::White;
			}

		s_checked_texture->LoadFromCanvas(canvas);

		vertices = {
			ae::Vector2f(c_border + 5.f, c_border + 5.f), ae::Vector2f(c_diameter - c_border - 5.f, c_border + 5.f),
			ae::Vector2f(c_diameter - c_border - 5.f, c_diameter - c_border - 5.f), ae::Vector2f(c_border + 5.f, c_diameter - c_border - 5.f)
		};

		s_check_square.reset(new ae::Sprite(ae::Vector2f(c_diameter, c_diameter)));
		s_check_square->SetTexture(*s_checked_texture);
		s_check_square->SetColor(GUI_COLOR_FRAME);
	}

	m_text.SetColor(GUI_COLOR_TEXT);
	m_text.SetFont(GUI_FONT);
	m_text.SetCharSize(15);

	// utility
	SetFamily(family);
	SetShape(shape);
}
GUIChooseBox::GUIChooseBox(const GUIChooseBox& copy) 
	: m_checked(copy.m_checked), m_hovered(copy.m_hovered), m_shape(copy.m_shape), m_text(copy.m_text)
{
	SetFamily(copy.m_family);
}
GUIChooseBox::GUIChooseBox(GUIChooseBox&& move) noexcept
	: m_checked(move.m_checked), m_hovered(move.m_hovered), m_shape(move.m_shape), m_text(std::move(move.m_text))
{
	std::string family = move.m_family;
	move.RemoveFromFamily();
	SetFamily(std::move(family));
}
GUIChooseBox::~GUIChooseBox() {
	RemoveFromFamily();
}


void GUIChooseBox::Draw(const ae::Matrix3x3& transform) const {
	
	// circle
	if (m_shape == Shape::Circle) {
		s_frame_circle->Draw(transform);

		if (m_hovered)
			s_hovered_content_circle->Draw(transform);
		else
			s_content_circle->Draw(transform);

		if (IsChecked())
			s_check_circle->Draw(transform);
	}

	// square
	else {
		s_frame_square->Draw(transform);

		if (m_hovered)
			s_hovered_content_square->Draw(transform);
		else
			s_content_square->Draw(transform);

		if (IsChecked())
			s_check_square->Draw(transform);
	}

	// text
	const ae::FloatRect& bounds = m_text.GetBounds();
	ae::Vector2f text_offset(
		c_diameter + c_text_padding,
		std::floor((c_diameter - bounds.top) / 2.f)

	);
	m_text.Draw(transform.GetTranslated(text_offset));
}
ae::FloatRect GUIChooseBox::GetBounds() const {
	const ae::FloatRect& bounds = m_text.GetBounds();
	return ae::FloatRect(
		0.f, 0.f,
		c_diameter
		+ (m_text.GetString().size() == 0 ? 0.f : (c_text_padding + bounds.left + bounds.width)), 
		
		std::max(
			c_diameter, 
			std::floor((c_diameter + bounds.top) / 2.f) + bounds.height
		)
	);
}

bool GUIChooseBox::HandleEvent(const ae::EventMouseButtonPressed& event, const ae::Vector2f& box_position) {
	if (
		event.GetButton() == ae::MouseButton::Left &&
		IsHovered(box_position) &&
		(m_allow_manual_uncheck || !IsChecked()) // don't return true if cannot be checked
	) {
		SetChecked(!IsChecked());
		return true;
	}

	return false;
}
void GUIChooseBox::HandleEvent(const ae::EventMouseMoved& event, const ae::Vector2f& box_position) {
	m_hovered = IsHovered(box_position);
}

bool GUIChooseBox::IsHovered(const ae::Vector2f& box_position) const {
	ae::Vector2f mouse = ae::Input.Mouse.GetPosition();

	return (
		mouse.x >= box_position.x && mouse.y >= box_position.y &&
		mouse.x < box_position.x + GetBounds().width && mouse.y < box_position.y + GetBounds().height
	);
}

void GUIChooseBox::SetFamily(const std::string& family) {
	if (m_family == family)
		return;

	if (family == std::string()) {
		RemoveFromFamily();
		return;
	}

	// remove from current family
	RemoveFromFamily();

	// find family
	auto family_it = s_families.find(family);
	if (family_it == s_families.end()) {
		s_families.emplace(family, std::vector<GUIChooseBox*>{this});
		m_checked = false;
		m_family = family;
		return;
	}

	// emplace in family
	family_it->second.push_back(this);
	m_checked = false;
	m_family = family;
}
void GUIChooseBox::RemoveFromFamily() {

	if (m_family == std::string())
		return;

	// find family
	auto family_it = s_families.find(m_family);
	if (family_it == s_families.end()) {
		m_family = std::string();
		return;
	}

	// find box
	std::vector<GUIChooseBox*>& family = family_it->second;

	auto box_it = std::find(family.begin(), family.end(), this);
	if (box_it != family.end()) {
		family.erase(box_it);
		m_family = std::string();
		m_checked = false;
	}
}
const std::string& GUIChooseBox::GetFamily() const { return m_family; }

void GUIChooseBox::SetShape(Shape shape) { m_shape = shape; }
GUIChooseBox::Shape GUIChooseBox::GetShape() const { return m_shape; }

void GUIChooseBox::SetString(const std::u32string& u32_string) {
	m_text.SetString(u32_string);
}
void GUIChooseBox::SetCharSize(unsigned int char_size) {
	m_text.SetCharSize(char_size);
}
void GUIChooseBox::SetBold(unsigned char strength) {
	m_text.SetBold(strength);
}


void GUIChooseBox::SetChecked(bool check) {
	if (m_checked != check) {

		// update family if new one is checked
		if (check && m_family != std::string()) {
			auto& family = s_families.find(m_family)->second;
			for (GUIChooseBox* box : family)
				box->m_checked = false;
		}
	}
		
	m_checked = check;
}
bool GUIChooseBox::IsChecked() const { return m_checked; }

void GUIChooseBox::AllowManualUncheck(bool allow) {
	m_allow_manual_uncheck = allow;
}