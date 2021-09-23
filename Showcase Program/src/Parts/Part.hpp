
#pragma once

#include <Aether.hpp>

class Part {
public:
	Part() = default;
	Part(const ae::FloatRect& bounds, const std::string& local_interface) : m_bounds(bounds), m_local_interface(local_interface) {}
	virtual ~Part() {}

	virtual void Draw(const ae::Matrix3x3& transform) = 0;
	virtual void Update(float deltatime) {};

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) { return false; }
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {}
	virtual void HandleEvent(ae::EventMouseButtonReleased& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {}

	virtual bool HandleEvent(ae::EventTextEntered& event, const ae::Matrix3x3& view, const ae::Vector2f& model) { return false; }
	virtual bool HandleEvent(ae::EventKeyPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) { return false; }
	virtual bool HandleEvent(ae::EventKeyRepeated& event, const ae::Matrix3x3& view, const ae::Vector2f& model) { return false; }

	const ae::FloatRect& GetBounds() const { return m_bounds; }
	const std::string& GetLocalInterface() const { return m_local_interface; }

private:
	ae::FloatRect m_bounds;
	std::string m_local_interface;
};