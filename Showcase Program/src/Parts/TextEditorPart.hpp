
#pragma once

#include <Aether.hpp>
#include "Part.hpp"
#include "../GUI/GUI.hpp"
#include "../LocalInterface.hpp"

class TextEditorInterface : public LocalInterface {
public:
	TextEditorInterface(
		const std::function<void(int)>& button_callback,
		const std::function<void(int, bool)>& box_callback
	);

	virtual void Draw(const ae::Matrix3x3& transform) override;
	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) override;
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) override;

private:
	ae::Text m_title;

	std::function<void(int)> m_button_callback;
	std::function<void(int, bool)> m_box_callback;

	GUIButton m_buttons[3];
	GUIChooseBox m_boxes[4];
};

class TextEditorPart : public Part {
public:
	TextEditorPart();
	~TextEditorPart();

	virtual void Draw(const ae::Matrix3x3& transform) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual bool HandleEvent(ae::EventTextEntered& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual bool HandleEvent(ae::EventKeyPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual bool HandleEvent(ae::EventKeyRepeated& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;

private:
	ae::Text m_editor;
	bool m_typing = false;
	ae::Vector2f m_mark_pos;
	int m_mark_index = -1;
	ae::Shape m_bounds, m_mark_shape;

	void AppendString(const std::u32string& u32string);
	void DeleteChar();
	void UpdateBounds();

	bool HandleKeyEvent(const ae::KeyboardKey& key);
};