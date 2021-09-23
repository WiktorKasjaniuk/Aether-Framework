#pragma once

#include <Aether.hpp>
#include "GUI/GUI.hpp"

class ShowcaseScene;
class InterfaceLayer : public ae::Layer {
	friend class ShowcaseScene;

public:
	InterfaceLayer(ShowcaseScene* scene);

	virtual void Draw() override;
	virtual void HandleEvent(ae::Event& event) override;

private:
	ShowcaseScene* m_scene;
	ae::FrameSprite m_framesprite;

	// texts
	ae::Text m_camera_manual_text, m_info_text;

	// framebuffer filters
	ae::Text m_framebuffer_options_text;
	GUIChooseBox m_framebuffer_options_boxes[3];
	ae::Vector2f m_framebuffer_options_offsets[3];
	std::int8_t m_framebuffer_options_choice = -1;

	// local interface
	ae::Vector2f m_local_interface_position = ae::Vector2f(10.f, 150.f);

	// play music
	ae::Music m_music;
	GUIChooseBox m_music_box;
	GUISlider m_music_sliders[2];
	ae::Vector2f m_music_offsets[3];

	void UpdateCamera() const;
	bool UpdateCameraScale(float scroll_offset) const;
	void UpdateInfo();
};