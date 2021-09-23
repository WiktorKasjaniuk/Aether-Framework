
#pragma once

#include <Aether.hpp>
#include "GUI/GUI.hpp"

#include "BackgroundLayer.hpp"
#include "MainLayer.hpp"
#include "InterfaceLayer.hpp"

class ShowcaseScene : public ae::Scene {
public:
	ShowcaseScene();
	~ShowcaseScene();
	virtual void Update() override;

	ae::Framebuffer& GetContentFramebuffer();
	const std::vector<MainLayer::PositionedPart>& GetParts() const;

private:
	BackgroundLayer* m_background_layer;
	MainLayer* m_main_layer;
	InterfaceLayer* m_interface_layer;

	ae::Framebuffer m_content_framebuffer { true };
};

