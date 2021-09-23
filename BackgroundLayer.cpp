
#include "BackgroundLayer.hpp"
#include "ShowcaseScene.hpp"

BackgroundLayer::BackgroundLayer(ShowcaseScene* scene) : m_scene(scene), ae::Layer() {
	m_background.SetTexture(ae::AssetManager.GetTexture("background"));
}

void BackgroundLayer::Draw() {
	m_scene->GetContentFramebuffer().Bind();
	m_scene->GetContentFramebuffer().Clear();

	m_background.Draw(ae::Camera.GetProjViewMatrix());

	m_scene->GetContentFramebuffer().Unbind();
}
