
#pragma once

#include <Aether.hpp>
#include "Part.hpp"
#include "../GUI/GUI.hpp"
#include "../LocalInterface.hpp"

class ImageEditorPart;
class ImageEditorInterface : public LocalInterface {
	friend class ImageEditorPart;

public:
	ImageEditorInterface(
		const std::function<void(int slider, int value)>& slider_callback,
		const std::function<void(int button)>& button_callback
	);

	virtual void Draw(const ae::Matrix3x3& transform) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) override;
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) override;
	virtual void HandleEvent(ae::EventMouseButtonReleased& event, const ae::Vector2f& position) override;

private:
	ae::Text m_title, m_note;
	GUISlider m_red, m_green, m_blue, m_pencil;
	GUIButton m_import, m_export;

	std::function<void(int slider, int value)> m_slider_callback;
	std::function<void(int button)> m_button_callback;
};
class ImageEditorPart : public Part {
public:
	ImageEditorPart();
	~ImageEditorPart();

	virtual void Draw(const ae::Matrix3x3& transform) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;

private:
	ImageEditorInterface* m_interface;

	ae::Text m_description;

	ae::TextureCanvas m_canvas;
	ae::Sprite m_sprite;
	ae::Shape m_border;

	int m_pencil = 1;
	int m_red = 0, m_green = 0, m_blue = 0;

	void DrawToCanvas(const ae::Vector2ui& point);
};