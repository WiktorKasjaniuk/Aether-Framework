
#include "LocalInterface.hpp"

std::string LocalInterface::s_current;
std::map<std::string, std::unique_ptr<LocalInterface>> LocalInterface::s_interfaces;

void LocalInterface::Delete(const std::string& name) {
	auto it = s_interfaces.find(name);
	if (it != s_interfaces.end())
		s_interfaces.erase(it);
}
void LocalInterface::SetCurrent(const std::string& name) {
	s_current = name;
}

void LocalInterface::DrawCurrent(const ae::Matrix3x3& transform) {
	auto it = s_interfaces.find(s_current);
	if (it != s_interfaces.end())
		it->second->Draw(transform);
}

bool LocalInterface::HandleEventCurrent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) {
	auto it = s_interfaces.find(s_current);

	if (it != s_interfaces.end())
		return it->second->HandleEvent(event, position);

	return false;
}
void LocalInterface::HandleEventCurrent(ae::EventMouseButtonReleased& event, const ae::Vector2f& position) {
	auto it = s_interfaces.find(s_current);

	if (it != s_interfaces.end())
		it->second->HandleEvent(event, position);
}
void LocalInterface::HandleEventCurrent(ae::EventMouseMoved& event, const ae::Vector2f& position) {
	auto it = s_interfaces.find(s_current);

	if (it != s_interfaces.end())
		it->second->HandleEvent(event, position);
}