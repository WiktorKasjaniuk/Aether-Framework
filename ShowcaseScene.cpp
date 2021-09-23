
#include "ShowcaseScene.hpp"

// ShowcaseScene
ShowcaseScene::ShowcaseScene() : ae::Scene() {
	
	// application icons
	ae::TextureCanvas icon16, icon32, icon48, cursor;

	if (
		icon16.Create("Assets/icon16.png") &&
		icon32.Create("Assets/icon32.png") &&
		icon48.Create("Assets/icon48.png")
	)
		ae::Window.SetIcon(icon16, icon32, icon48);

	if (cursor.Create("Assets/cursor.png"))
		ae::Cursor.UploadDefaultIcon(ae::CursorMode::Arrow, cursor);

	// framebuffer
	m_content_framebuffer.Bind();
	m_content_framebuffer.Resize(ae::Window.GetContextSize());

	// assets
	ae::AssetManager.LoadTexture("background", "Assets/background.png");
	ae::AssetManager.LoadTexture("bounce_tile", "Assets/bounce_tile.png");

	ae::AssetManager.LoadFont("urbanist", "Assets/Urbanist-Regular.ttf");

	ae::AssetManager.LoadSoundBuffer("bounce_tile_bounce", "Assets/bounce_tile_bounce.wav");

	ae::AssetManager.LoadShader("framebuffer", ae::Shader::LoadMode::FromFile, "Assets/framebuffer.vsh", "Assets/framebuffer.fsh");
	ae::AssetManager.LoadShader("vertex_grabber", ae::Shader::LoadMode::FromFile, "Assets/vertex_grabber.vsh", "Assets/vertex_grabber.fsh");

	// layers
	m_background_layer = &ae::LayerManager.CreateLayer<BackgroundLayer>(true, this);
	m_main_layer = &ae::LayerManager.CreateLayer<MainLayer>(true, this);
	m_interface_layer = &ae::LayerManager.CreateLayer<InterfaceLayer>(true, this);
}
ShowcaseScene::~ShowcaseScene() {
	ae::LayerManager.RemoveLayer(m_background_layer);
	ae::LayerManager.RemoveLayer(m_main_layer);
	ae::LayerManager.RemoveLayer(m_interface_layer);
}

void ShowcaseScene::Update() {
	m_interface_layer->UpdateCamera();
	m_interface_layer->UpdateInfo();
	m_main_layer->Update();
}
ae::Framebuffer& ShowcaseScene::GetContentFramebuffer() { return m_content_framebuffer; }
const std::vector<MainLayer::PositionedPart>& ShowcaseScene::GetParts() const { return m_main_layer->m_parts; }