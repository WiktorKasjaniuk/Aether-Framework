
#pragma once

#include <Aether.hpp>

class LocalInterface {
	friend class InterfaceLayer;

public:
	virtual ~LocalInterface() {};
	virtual void Draw(const ae::Matrix3x3& transform) = 0;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) { return false; };
	virtual void HandleEvent(ae::EventMouseButtonReleased& event, const ae::Vector2f& position) {};
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) {};

	template <typename InterfaceType, typename ...ArgsType>
	static InterfaceType* Create(const std::string& name, ArgsType ...args) {
		auto it = s_interfaces.find(name);

		if (it == s_interfaces.end()) {
			InterfaceType* interface = new InterfaceType(std::forward<ArgsType>(args)...);
			s_interfaces.emplace(name, interface);
			return interface;
		}

		else
			return nullptr;
	}
	static void Delete(const std::string& name);
	static void SetCurrent(const std::string& name);

protected:
	LocalInterface() = default;

private:
	static std::string s_current;
	static std::map<std::string, std::unique_ptr<LocalInterface>> s_interfaces;

	LocalInterface(const LocalInterface&) = delete;
	LocalInterface(LocalInterface&&) = delete;

	static void DrawCurrent(const ae::Matrix3x3& transform);

	static bool HandleEventCurrent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position);
	static void HandleEventCurrent(ae::EventMouseButtonReleased& event, const ae::Vector2f& position);
	static void HandleEventCurrent(ae::EventMouseMoved& event, const ae::Vector2f& position);
};