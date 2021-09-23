
#include "TextEditorPart.hpp"

constexpr float c_button_padding = 40.f, c_box_padding = 30.f, c_box_offset = 140.f;

TextEditorInterface::TextEditorInterface(
	const std::function<void(int)>& button_callback,
	const std::function<void(int, bool)>& box_callback
) 
	: LocalInterface(), m_button_callback(button_callback), m_box_callback(box_callback)
{
	// title
	m_title.SetCharSize(20);
	m_title.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_title.SetString(U"== Text Editor ==");

	// clipboard buttons
	m_buttons[0].SetString(U"Copy to clipboard");
	m_buttons[0].SetSize(ae::Vector2f(257.f, 30.f));

	m_buttons[1].SetString(U"Paste from clipboard");
	m_buttons[1].SetSize(ae::Vector2f(257.f, 30.f));

	m_buttons[2].SetString(U"Log as error to \"error_log.txt\"");
	m_buttons[2].SetSize(ae::Vector2f(257.f, 30.f));

	// style choose boxes
	m_boxes[0].SetShape(GUIChooseBox::Shape::Square);
	m_boxes[0].SetString(U"Bold");

	m_boxes[1].SetShape(GUIChooseBox::Shape::Square);
	m_boxes[1].SetString(U"Italic Shear");

	m_boxes[2].SetShape(GUIChooseBox::Shape::Square);
	m_boxes[2].SetString(U"Underline");

	m_boxes[3].SetShape(GUIChooseBox::Shape::Square);
	m_boxes[3].SetString(U"Strikeline");
}

void TextEditorInterface::Draw(const ae::Matrix3x3& transform) {
	m_title.Draw(transform.GetTranslated(ae::Vector2f(0.f, -10.f)));

	for (int i(0); i < 3; i++)
		m_buttons[i].Draw(transform.GetTranslated(ae::Vector2f(0.f, i * c_button_padding)));

	for (int i(0); i < 4; i++)
		m_boxes[i].Draw(transform.GetTranslated(ae::Vector2f(0.f, c_box_offset + i * c_box_padding)));
}
bool TextEditorInterface::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) {

	for (int i(0); i < 3; i++)
		if (m_buttons[i].HandleEvent(event, ae::Vector2f(position.x, position.y + i * c_button_padding))) {
			if (m_button_callback)
				m_button_callback(i);
			return true;
		}

	for (int i(0); i < 4; i++)
		if (m_boxes[i].HandleEvent(event, ae::Vector2f(position.x, position.y + c_box_offset + i * c_box_padding))) {
			if (m_box_callback)
				m_box_callback(i, m_boxes[i].IsChecked());
			return true;
		}

	return false;
}
void TextEditorInterface::HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) {

	for (int i(0); i < 3; i++)
		m_buttons[i].HandleEvent(event, ae::Vector2f(position.x, position.y + i * c_button_padding));

	for (int i(0); i < 4; i++)
		m_boxes[i].HandleEvent(event, ae::Vector2f(position.x, position.y + c_box_offset + i * c_box_padding));
}

TextEditorPart::TextEditorPart() 
	: Part(ae::FloatRect(-100.f, -100.f, 1000.f, 1000.f), "text_editor")
{
	// editor
	m_editor.SetCharSize(50);
	m_editor.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_editor.SetString(U"Text Editor...\nClick somewhere here to set\na mark and start typing!\nTo use clipboard check the interface\nto the left of your screen.");

	// bounds
	const ae::FloatRect& text_bounds = m_editor.GetBounds();

	m_bounds.SetColor(ae::Color(30, 140, 255, 100));
	m_bounds.SetIndices({ 0,1,2, 2,3,0 });
	m_bounds.SetVertices({
		ae::VertexPos(text_bounds.left, text_bounds.top),
		ae::VertexPos(text_bounds.left + text_bounds.width, text_bounds.top),
		ae::VertexPos(text_bounds.left + text_bounds.width, text_bounds.top + text_bounds.height),
		ae::VertexPos(text_bounds.left, text_bounds.top + text_bounds.height)
	});

	// mark
	m_mark_shape.SetColor(ae::Color(255, 0, 0));
	m_mark_shape.SetIndices({ 0,1,2, 2,3,0 });
	m_mark_shape.SetVertices({
		ae::VertexPos(-1.f, -40.f), ae::VertexPos(1.f, -40.f),
		ae::VertexPos(1.f, 5.f), ae::VertexPos(-1.f, 5.f)
	});

	// local interface
	LocalInterface::Create<TextEditorInterface>(
		"text_editor",
		[this](int button) {
			if (button == 0)
				ae::Clipboard.SetSystemStorage(ae::utf::ConvertToUtf8(m_editor.GetString()));
			else if (button == 1)
				AppendString(ae::utf::ConvertToUtf32(ae::Clipboard.GetSystemStorage()));
			else
				ae::LogError(ae::utf::ConvertToUtf8(m_editor.GetString()), false);
		},
		[this](int box, bool value) {
			if (box == 0) m_editor.SetBold(value ? 2 : 0);
			else if (box == 1) m_editor.SetItalicShear(value);
			else if (box == 2) m_editor.SetUnderline(value);
			else m_editor.SetStrikeline(value);

			UpdateBounds();
		}
	);
}
TextEditorPart::~TextEditorPart() {
	LocalInterface::Delete("text_editor");
}

void TextEditorPart::TextEditorPart::Draw(const ae::Matrix3x3& transform) {
	m_bounds.Draw(transform);
	m_editor.Draw(transform);

	float seconds = ae::Application.GetRunTime().GetSeconds();
	bool animate = fmod(seconds, 1.f) < 0.5f; // every 0.5s make the marker visible

	if (m_typing && animate)
		m_mark_shape.Draw(transform.GetTranslated(m_mark_pos));
}
bool TextEditorPart::TextEditorPart::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	// transform mouse to world coordinates
	ae::Vector2f fixed_mouse = view.GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());
	ae::Vector2f left_top = model + m_bounds.GetVertices()[0].position;
	ae::Vector2f right_bottom = model + m_bounds.GetVertices()[2].position;

	if (
		fixed_mouse.x >= left_top.x && fixed_mouse.x < right_bottom.x &&
		fixed_mouse.y >= left_top.y && fixed_mouse.y < right_bottom.y
	) {
		fixed_mouse -= model;
		ae::Text::CharMetrics metrics = m_editor.CalculateNearestCharMetrics(fixed_mouse);

		if (abs(fixed_mouse.x - metrics.last_advance.x) > abs(fixed_mouse.x - metrics.advance.x)) {
			m_mark_pos = ae::Vector2f(
				metrics.advance.x,
				metrics.advance.y
			);
			m_mark_index = metrics.index;
		}
		else {
			m_mark_pos = ae::Vector2f(
				metrics.last_advance.x,
				metrics.last_advance.y
			);
			m_mark_index = std::max(0, static_cast<int>(metrics.index) - 1);
		}

		m_typing = true;
		return true;
	}

	m_typing = false;
	return false;
}
bool TextEditorPart::TextEditorPart::HandleEvent(ae::EventTextEntered& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	if (m_typing) {
		std::u32string str;
		str += event.GetCharacter();
		AppendString(std::move(str));
		return true;
	}
	return false;
}
bool TextEditorPart::HandleEvent(ae::EventKeyPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {
	return HandleKeyEvent(event.GetKey());
}
bool TextEditorPart::HandleEvent(ae::EventKeyRepeated& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {
	return HandleKeyEvent(event.GetKey());
}

bool TextEditorPart::HandleKeyEvent(const ae::KeyboardKey& key) {
	if (m_typing) {
		switch (key) {
			default: break;
			case ae::KeyboardKey::Enter: AppendString(U"\n"); break;
			case ae::KeyboardKey::Backspace: DeleteChar(); break;
		}
		return true;
	}
	return false;
}

void TextEditorPart::AppendString(const std::u32string& u32string) {
	std::u32string string = m_editor.GetString();
	string = string.substr(0, m_mark_index + 1) + u32string + string.substr(m_mark_index + 1);
	m_editor.SetString(std::move(string));

	UpdateBounds();

	m_mark_index += u32string.size();
	ae::Text::CharMetrics metrics = m_editor.CalculateCharMetrics(m_mark_index);
	m_mark_pos = metrics.advance;
}
void TextEditorPart::DeleteChar() {
	if (m_mark_index == -1)
		return;

	std::u32string string = m_editor.GetString();
	string.erase(string.begin() + m_mark_index);
	m_editor.SetString(std::move(string));

	UpdateBounds();

	m_mark_index--;
	ae::Text::CharMetrics metrics = m_editor.CalculateCharMetrics(std::max(0, m_mark_index));
	m_mark_pos = m_mark_index == -1 ? metrics.last_advance : metrics.advance;
}
void TextEditorPart::UpdateBounds() {
	const ae::FloatRect& text_bounds = m_editor.GetBounds();
	m_bounds.SetVertices({
		ae::VertexPos(text_bounds.left, text_bounds.top),
		ae::VertexPos(text_bounds.left + text_bounds.width, text_bounds.top),
		ae::VertexPos(text_bounds.left + text_bounds.width, text_bounds.top + text_bounds.height),
		ae::VertexPos(text_bounds.left, text_bounds.top + text_bounds.height)
	});
}