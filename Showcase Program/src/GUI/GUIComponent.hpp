
#pragma once

#include <Aether.hpp>

class GUIComponent {
public:
	virtual void Draw(const ae::Matrix3x3& transform) const = 0;
	virtual ae::FloatRect GetBounds() const = 0;
};
