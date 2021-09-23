
#include <Aether.hpp>
#include "ShowcaseScene.hpp"

int main() {
	ae::Application.Initialize(ae::Vector2f(1200.f, 800.f), "Aether-Fueled Engine Application");
	ae::SceneId scene = ae::SceneManager.RegisterScene<ShowcaseScene>();

	ae::SceneManager.NextScene(scene);
	ae::Application.Run();
}