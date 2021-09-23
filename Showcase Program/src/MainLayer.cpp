
#include "MainLayer.hpp"
#include "ShowcaseScene.hpp"

#include "Parts/MoveCameraPart.hpp"
#include "Parts/VertexGrabberPart.hpp"
#include "Parts/ImageEditorPart.hpp"
#include "Parts/BouncePart.hpp"
#include "Parts/TextEditorPart.hpp"
#include "Parts/StencilPart.hpp"

MainLayer::MainLayer(ShowcaseScene* scene) : m_scene(scene), ae::Layer() {
	m_parts.emplace_back(std::unique_ptr<Part>(new MoveCameraPart()), ae::Vector2f(200.f, 300.f));
	m_parts.emplace_back(std::unique_ptr<Part>(new VertexGrabberPart()), ae::Vector2f(1600.f, 300.f));
	m_parts.emplace_back(std::unique_ptr<Part>(new ImageEditorPart()), ae::Vector2f(200.f, 1000.f));
	m_parts.emplace_back(std::unique_ptr<Part>(new BouncePart()), ae::Vector2f(1200.f, 1000.f));
	m_parts.emplace_back(std::unique_ptr<Part>(new TextEditorPart()), ae::Vector2f(50.f, 2000.f));
	m_parts.emplace_back(std::unique_ptr<Part>(new StencilPart()), ae::Vector2f(1200.f, 2000.f));
}

void MainLayer::Draw() {
	m_scene->GetContentFramebuffer().Bind();

	const ae::Matrix3x3& proj_view = ae::Camera.GetProjViewMatrix();
	for (PositionedPart& part : m_parts)
		part.part->Draw(proj_view.GetTranslated(part.position));

	m_scene->GetContentFramebuffer().Unbind();
}
void MainLayer::HandleEvent(ae::Event& event) {
	
	const ae::Matrix3x3& view = ae::Camera.GetViewMatrix();

	event.Dispatch<ae::EventMouseButtonPressed>(
		[this, &view](auto& event) -> bool {
			for (PositionedPart& part : m_parts)
				if (part.part->HandleEvent(event, view, part.position))
					return true;

			return false;
		}
	);

	event.Dispatch<ae::EventMouseMoved>(
		[this, &view](auto& event) -> bool {
			for (PositionedPart& part : m_parts)
				part.part->HandleEvent(event, view, part.position);

			return false;
		}
	);
	
	event.Dispatch<ae::EventMouseButtonReleased>(
		[this, &view](auto& event) -> bool {
			for (PositionedPart& part : m_parts)
				part.part->HandleEvent(event, view, part.position);

			return false;
		}
	);

	event.Dispatch<ae::EventTextEntered>(
		[this, &view](auto& event) -> bool {
			for (PositionedPart& part : m_parts)
				if (part.part->HandleEvent(event, view, part.position))
					return true;

			return false;
		}
	);

	event.Dispatch<ae::EventKeyPressed>(
		[this, &view](auto& event) -> bool {
			for (PositionedPart& part : m_parts)
				if (part.part->HandleEvent(event, view, part.position))
					return true;

			return false;
		}
	);
	event.Dispatch<ae::EventKeyRepeated>(
		[this, &view](auto& event) -> bool {
			for (PositionedPart& part : m_parts)
				if (part.part->HandleEvent(event, view, part.position))
					return true;

			return false;
		}
	);
}
void MainLayer::Update() {
	float deltatime = ae::Application.GetTickTime().GetSeconds();

	for (PositionedPart& part : m_parts)
		part.part->Update(deltatime);
}
