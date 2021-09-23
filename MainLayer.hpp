#pragma once

#include <Aether.hpp>
#include "GUI/GUI.hpp"

#include "Parts/Part.hpp"

class ShowcaseScene;
class MainLayer : public ae::Layer {
	friend class ShowcaseScene;

public:
	MainLayer(ShowcaseScene* scene);

	virtual void Draw() override;
	virtual void HandleEvent(ae::Event& event) override;

	void Update();

private:
	ShowcaseScene* m_scene;

	// parts
	struct PositionedPart {
		std::unique_ptr<Part> part;
		ae::Vector2f position;

		PositionedPart(std::unique_ptr<Part>&& part, ae::Vector2f&& position)
			: part(std::move(part)), position(position) {}
	};
	std::vector<PositionedPart> m_parts;
};