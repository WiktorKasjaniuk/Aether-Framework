
#include "ImageEditorPart.hpp"

constexpr float c_slider_padding = 24.f, c_slider_offset = 70.f;
constexpr float c_button_padding = 40.f, c_button_offset = 180.f;
constexpr int c_canvas_size = 500;

ImageEditorInterface::ImageEditorInterface(
	const std::function<void(int slider, int value)>& slider_callback,
	const std::function<void(int button)>& button_callback
)
	: m_slider_callback(slider_callback), m_button_callback(button_callback)
{
	// title
	m_title.SetCharSize(20);
	m_title.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_title.SetString(U"== Image Editor ==");

	// note
	m_note.SetCharSize(20);
	m_note.SetColor(ae::Color(200, 200, 200, 255));
	m_note.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_note.SetString(U"draw - Left Mouse Button\npick - Right Mouse Button");

	// rgb
	m_red.SetSize(ae::Vector2f(257.f, 20.f));
	m_red.SetString(U"Red = 0");

	m_green.SetSize(ae::Vector2f(257.f, 20.f));
	m_green.SetString(U"Green = 0");

	m_blue.SetSize(ae::Vector2f(257.f, 20.f));
	m_blue.SetString(U"Blue = 0");

	// pencil
	m_pencil.SetSize(ae::Vector2f(257.f, 20.f));
	m_pencil.SetString(U"Pencil = 1");

	// import export
	m_import.SetString(U"Import from \"input_image.png\"");
	m_import.SetSize(ae::Vector2f(257.f, 30.f));

	m_export.SetString(U"Export to \"output_image.png\"");
	m_export.SetSize(ae::Vector2f(257.f, 30.f));
}

void ImageEditorInterface::Draw(const ae::Matrix3x3& transform) {
	m_title.Draw(transform);
	m_note.Draw(transform.GetTranslated(ae::Vector2f(0.f, 30.f)));

	// sliders
	ae::Vector2f offset;
	offset.y = c_slider_offset;
	m_red.Draw(transform.GetTranslated(offset));

	offset.y += c_slider_padding;
	m_green.Draw(transform.GetTranslated(offset));

	offset.y += c_slider_padding;
	m_blue.Draw(transform.GetTranslated(offset));

	offset.y += c_slider_padding;
	m_pencil.Draw(transform.GetTranslated(offset));

	// buttons
	offset.y = c_button_offset;
	m_import.Draw(transform.GetTranslated(offset));

	offset.y += c_button_padding;
	m_export.Draw(transform.GetTranslated(offset));
}

#define SLIDER_EVENT(event, position, slider, slider_id) slider.HandleEvent(event, ae::Vector2f(position.x, position.y + c_slider_offset + slider_id * c_slider_padding))
#define SLIDER_UPDATE(slider, mod, slider_id, return_line) \
	if (m_slider_callback) { \
			std::u32string string = \
				slider.GetString().substr(0, slider.GetString().find('=') + 2) \
				+ ae::utf::ConvertToUtf32(std::to_string(static_cast<int>(mod(slider.GetValue())))) \
			; \
		 \
			slider.SetString(std::move(string)); \
			m_slider_callback(slider_id, mod(slider.GetValue())); \
			return_line; \
	}

#define IF_SLIDER_EVENT(event, position, slider, slider_id, mod, return_line) \
	if (SLIDER_EVENT(event, position, slider, slider_id)) {\
		if (m_slider_callback) { \
			SLIDER_UPDATE(slider, mod, slider_id, return_line) \
		} \
	}

#define SLIDER_MOD_RGB(value) (value * 255.f)
#define SLIDER_MOD_PENCIL(value) (value * 15.f + 1.f)

bool ImageEditorInterface::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) {

	IF_SLIDER_EVENT(event, position, m_red, 0, SLIDER_MOD_RGB, return true)
	else IF_SLIDER_EVENT(event, position, m_green, 1, SLIDER_MOD_RGB, return true)
	else IF_SLIDER_EVENT(event, position, m_blue, 2, SLIDER_MOD_RGB, return true)
	else IF_SLIDER_EVENT(event, position, m_pencil, 3, SLIDER_MOD_PENCIL, return true)

	if (m_import.HandleEvent(event, ae::Vector2f(position.x, position.y + c_button_offset))) {
		if (m_button_callback) {
			m_button_callback(0);
			return true;
		}
	}
	else if (m_export.HandleEvent(event, ae::Vector2f(position.x, position.y + c_button_offset + c_button_padding)))
		if (m_button_callback) {
			m_button_callback(1);
			return true;
		}

	return false;
}
void ImageEditorInterface::HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) {
	SLIDER_EVENT(event, position, m_red, 0);
	SLIDER_UPDATE(m_red, SLIDER_MOD_RGB, 0, );

	SLIDER_EVENT(event, position, m_green, 1);
	SLIDER_UPDATE(m_green, SLIDER_MOD_RGB, 1, );

	SLIDER_EVENT(event, position, m_blue, 2);
	SLIDER_UPDATE(m_blue, SLIDER_MOD_RGB, 2, );

	SLIDER_EVENT(event, position, m_pencil, 3);
	SLIDER_UPDATE(m_pencil, SLIDER_MOD_PENCIL, 3, );

	m_import.HandleEvent(event, ae::Vector2f(position.x, position.y + c_button_offset));
	m_export.HandleEvent(event, ae::Vector2f(position.x, position.y + c_button_offset + c_button_padding));
}
void ImageEditorInterface::HandleEvent(ae::EventMouseButtonReleased& event, const ae::Vector2f& position) {
	m_red.HandleEvent(event, ae::Vector2f(position.x, position.y + c_slider_offset));
	m_green.HandleEvent(event, ae::Vector2f(position.x, position.y + c_slider_offset + c_slider_offset));
	m_blue.HandleEvent(event, ae::Vector2f(position.x, position.y + c_slider_offset + 2.f * c_slider_offset));
	m_pencil.HandleEvent(event, ae::Vector2f(position.x, position.y + c_slider_offset + 3.f * c_slider_offset));
}

ImageEditorPart::ImageEditorPart() 
	: Part(ae::FloatRect(-100.f, -100.f, 700.f, 700.f), "image_editor")
{
	// description
	m_description.SetCharSize(50);
	m_description.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_description.SetString(U"Image Editor");

	// canvas
	if (!m_canvas.Create("Assets/input_image.png")) {
		m_canvas.Create(ae::Vector2ui(2, 2), ae::Color::Nova);
		m_canvas.Px(0, 0) = ae::Color::Matrix;
		m_canvas.Px(1, 1) = ae::Color::Matrix;
	}
	m_canvas.Stretch(ae::Vector2ui(c_canvas_size, c_canvas_size));

	ae::AssetManager.LoadTexture("edited_image", m_canvas);
	m_sprite.SetTexture(ae::AssetManager.GetTexture("edited_image"));

	m_border.SetColor(ae::Color::Black);
	m_border.SetIndices({ 0,1,2, 2,3,0 });
	m_border.SetVertices({
		ae::VertexPos(-10.f, -10.f), ae::VertexPos(c_canvas_size + 10.f, -10.f),
		ae::VertexPos(c_canvas_size + 10.f, c_canvas_size + 10.f), ae::VertexPos(-10.f, c_canvas_size + 10.f)
	});

	// local interface
	m_interface = LocalInterface::Create<ImageEditorInterface>(
		"image_editor",
		
		[this](int slider, int value) {
			if (slider == 0) m_red = value;
			else if (slider == 1) m_green = value;
			else if (slider == 2) m_blue = value;
			else m_pencil = value;

			m_border.SetColor(ae::Color(m_red, m_green, m_blue, 255));
		},

		[this](int button) {
			if (button == 0) {
				if (!m_canvas.Create("Assets/input_image.png")) {
					m_canvas.Create(ae::Vector2ui(2, 2), ae::Color::Nova);
					m_canvas.Px(0, 0) = ae::Color::Matrix;
					m_canvas.Px(1, 1) = ae::Color::Matrix;
				}

				m_canvas.Stretch(ae::Vector2ui(c_canvas_size, c_canvas_size));

				ae::AssetManager.RemoveTexture("edited_image");
				ae::AssetManager.LoadTexture("edited_image", m_canvas);

				m_sprite.SetTexture(ae::AssetManager.GetTexture("edited_image"));
			}
			else
				m_canvas.SaveToFile("Assets/output_image.png");
		}
	);
}
ImageEditorPart::~ImageEditorPart() {
	LocalInterface::Delete("image_editor");
}
void ImageEditorPart::Draw(const ae::Matrix3x3 &transform) {
	m_description.Draw(transform.GetTranslated(ae::Vector2f(0.f, -40.f)));
	m_border.Draw(transform);
	m_sprite.Draw(transform);
}

bool ImageEditorPart::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	ae::Vector2f fixed_left_top = view.GetTranslated(model).GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());
	ae::Vector2f fixed_right_bottom = view.GetTranslated(model + ae::Vector2f(c_canvas_size, c_canvas_size)).GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());

	if (
		fixed_left_top.x >= 0.f && fixed_right_bottom.x < 0.f &&
		fixed_left_top.y >= 0.f && fixed_right_bottom.y < 0.f
	) {
		ae::Vector2ui point = view.GetTranslated(model).GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());

		if (event.GetButton() == ae::MouseButton::Right) {
			ae::Color c = m_canvas.Px(point.x, point.y);
			m_red = c.r;
			m_green = c.g;
			m_blue = c.b;

			m_border.SetColor(ae::Color(c.r, c.g, c.b, 255)); // some will be drawn transparent
			m_interface->m_red.SetValue(c.r / 255.f);
			m_interface->m_green.SetValue(c.g / 255.f);
			m_interface->m_blue.SetValue(c.b / 255.f);
		}
		
		else if (event.GetButton() == ae::MouseButton::Left)
			DrawToCanvas(point);

		return true;
	}

	return false;
}

void ImageEditorPart::DrawToCanvas(const ae::Vector2ui& point) {

	ae::TextureCanvas pencil_canvas;
	pencil_canvas.Create(ae::Vector2ui(m_pencil, m_pencil), ae::Color(m_red, m_green, m_blue, 255));

	m_canvas.Paste(pencil_canvas, point - ae::Vector2ui(m_pencil / 2, m_pencil / 2));

	ae::AssetManager.RemoveTexture("edited_image");
	ae::AssetManager.LoadTexture("edited_image", m_canvas);
	m_sprite.SetTexture(ae::AssetManager.GetTexture("edited_image"));
}