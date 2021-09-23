#pragma once

#include <Aether.hpp>
#include "GUI/GUI.hpp"

class ShowcaseScene;
class BackgroundLayer : public ae::Layer {
	friend class ShowcaseScene;

public:
	BackgroundLayer(ShowcaseScene* scene);

	virtual void Draw() override;
	virtual void HandleEvent(ae::Event& event) override {}

private:
	ShowcaseScene* m_scene;

	ae::Sprite m_background;
};