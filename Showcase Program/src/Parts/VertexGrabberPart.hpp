
#pragma once

#include <Aether.hpp>
#include "Part.hpp"
#include "../GUI/GUI.hpp"
#include "../LocalInterface.hpp"

class VertexGrabberInterface : public LocalInterface {
public:
	VertexGrabberInterface(const std::function<void(ae::DrawMode)>& draw_mode_callback);

	virtual void Draw(const ae::Matrix3x3& transform) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) override;
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) override;

private:
	ae::Text m_title;
	GUIChooseBox m_draw_mode_boxes[3];
	std::function<void(ae::DrawMode)> m_draw_mode_callback;
};

class VertexGrabberPart : public Part {
public:
	VertexGrabberPart();
	~VertexGrabberPart();

	virtual void Draw(const ae::Matrix3x3& transform) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual void HandleEvent(ae::EventMouseButtonReleased& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;

private:
	ae::Text m_description, m_subdescription, m_error_text;

	ae::VertexArray<ae::VertexPos> m_grab_vertices;
	ae::VertexArray<ae::VertexPos> m_grab_point_vertices;
	std::vector<ae::Vector2f> m_grab_point_offsets;

	int m_grabbed_vertex = -1;
};