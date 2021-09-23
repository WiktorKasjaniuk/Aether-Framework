
#include "InterfaceLayer.hpp"
#include "ShowcaseScene.hpp"
#include "LocalInterface.hpp"

#define SCALE_TO_LISTENER(scale) (-800.f / pow(scale, 2))

#define NORMAL_TO_PITCH(normal) (normal + 0.5f)
#define PITCH_TO_NORMAL(pitch) (pitch - 0.5f)

#define NORMAL_TO_MUSIC_VOLUME(normal) (normal * 0.005f)

InterfaceLayer::InterfaceLayer(ShowcaseScene* scene) : m_scene(scene), ae::Layer() {

	m_framesprite.SetFramebuffer(scene->GetContentFramebuffer());
	m_framesprite.SetSize(scene->GetContentFramebuffer().GetTexture().GetSize());

	// positioning
	ae::Vector2f hcsize = ae::Vector2f(ae::Window.GetContextSize()) / 2.f;
	ae::Camera.SetOrigin(hcsize);
	ae::Listener.SetPosition(ae::Vector3f(hcsize.x, hcsize.y, SCALE_TO_LISTENER(ae::Camera.GetScale().x)));
	ae::Listener.SetGlobalVolume(50.f);
	
	// camera manual
	m_camera_manual_text.SetCharSize(20);
	m_camera_manual_text.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_camera_manual_text.SetString(U"Camera Manual:\nLShift + \nMovement[W, A, S, D],\nScale[Scroll Up / Down],\nRotation[Q, E],\nShear[C, V]");

	// info
	m_info_text.SetCharSize(20);
	m_info_text.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_info_text.SetString(U"FPS = 0\nRunTime = 0s");

	// framebuffer filters
	m_framebuffer_options_text.SetCharSize(20);
	m_framebuffer_options_text.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_framebuffer_options_text.SetString(U"Framebuffer Filters:");

	m_framebuffer_options_boxes[0].SetShape(GUIChooseBox::Shape::Circle);
	m_framebuffer_options_boxes[0].SetFamily("fb_filters");
	m_framebuffer_options_boxes[0].SetString(U"Night Vision");

	m_framebuffer_options_boxes[1].SetShape(GUIChooseBox::Shape::Circle);
	m_framebuffer_options_boxes[1].SetFamily("fb_filters");
	m_framebuffer_options_boxes[1].SetString(U"Grayscale");

	m_framebuffer_options_boxes[2].SetShape(GUIChooseBox::Shape::Circle);
	m_framebuffer_options_boxes[2].SetFamily("fb_filters");
	m_framebuffer_options_boxes[2].SetString(U"Position");

	ae::FloatRect bounds = m_framebuffer_options_boxes[2].GetBounds();
	float y_offset = -(bounds.top + bounds.height + 10.f);
	m_framebuffer_options_offsets[2] = ae::Vector2f(
		-(bounds.left + bounds.width + 10.f),
		y_offset
	);

	bounds = m_framebuffer_options_boxes[1].GetBounds();
	m_framebuffer_options_offsets[1] = ae::Vector2f(
		m_framebuffer_options_offsets[2].x - (bounds.left + bounds.width + 10.f),
		y_offset
	);

	bounds = m_framebuffer_options_boxes[0].GetBounds();
	m_framebuffer_options_offsets[0] = ae::Vector2f(
		m_framebuffer_options_offsets[1].x - (bounds.left + bounds.width + 10.f),
		y_offset
	);

	// play music
	m_music.SetFile("Assets/odd_things_happening.ogg");
	m_music.SetLoop(true);

	m_music_box.SetShape(GUIChooseBox::Shape::Square);
	m_music_box.SetString(U"Play Music");
	
	m_music_sliders[0].SetString(U"Volume");
	m_music_sliders[0].SetValue(1.f);

	m_music_sliders[1].SetString(U"Pitch");
	m_music_sliders[1].SetValue(PITCH_TO_NORMAL(1.f));

	bounds = m_music_sliders[1].GetBounds();
	m_music_offsets[2] = ae::Vector2f(
		-(bounds.left + bounds.width + 10.f),
		10.f
	);

	bounds = m_music_sliders[0].GetBounds();
	m_music_offsets[1] = ae::Vector2f(
		m_music_offsets[2].x - (bounds.left + bounds.width + 10.f),
		10.f
	);

	bounds = m_music_box.GetBounds();
	m_music_offsets[0] = ae::Vector2f(
		m_music_offsets[1].x - (bounds.left + bounds.width + 10.f),
		10.f
	);
}

void InterfaceLayer::Draw() {
	const ae::Matrix3x3& proj = ae::Camera.GetProjMatrix();
	ae::Vector2f context_size = ae::Window.GetContextSize();

	// content framebuffer
	m_framesprite.Draw(
		[this, &proj](const ae::Texture* texture, const ae::Color& color, const ae::VertexArray<ae::VertexPosTex>& vertices) -> void {

			ae::Shader* shader = &ae::AssetManager.GetShader("framebuffer");

			bool shader_loaded = *shader != ae::AssetManager.GetDefaultShader();

			if (!shader_loaded)
				shader = &ae::DefaultAssets->framesprite_shader;

			shader->Bind();

			texture->Bind(0);
			shader->SetUniform.Sampler2D("u_texture", 0);
			shader->SetUniform.Mat3x3("u_mvp", proj.GetArray());
			if (shader_loaded)
				shader->SetUniform.Float("u_filter", static_cast<float>(m_framebuffer_options_choice));
			shader->SetUniform.Vec4f("u_color", color);

			vertices.Bind();
			vertices.Draw();
		}
	);

	// local interface
	LocalInterface::DrawCurrent(proj.GetTranslated(m_local_interface_position));

	// camera manual
	ae::Vector2f camera_manual_offset(
		10.f,
		ae::Window.GetContextSize().y - m_camera_manual_text.GetBounds().height - m_camera_manual_text.GetBounds().top - 10.f
	);
	m_camera_manual_text.Draw(proj.GetTranslated(camera_manual_offset));

	// info
	ae::Vector2f info_offset(
		10.f,
		-m_info_text.GetBounds().top + 10.f
	);
	m_info_text.Draw(proj.GetTranslated(info_offset));

	// fb options
	const ae::FloatRect& fb_options_bounds = m_framebuffer_options_text.GetBounds();
	ae::Vector2f fb_text_options_offset(
		context_size.x + m_framebuffer_options_offsets[0].x - (fb_options_bounds.left + fb_options_bounds.width + 10.f),
		context_size.y - (fb_options_bounds.top + fb_options_bounds.height + 10.f)
	);

	m_framebuffer_options_text.Draw(proj.GetTranslated(fb_text_options_offset));

	for (int i(0); i < 3; i++)
		m_framebuffer_options_boxes[i].Draw(proj.GetTranslated(context_size + m_framebuffer_options_offsets[i]));

	// play music
	m_music_box.Draw(proj.GetTranslated(ae::Vector2f(context_size.x + m_music_offsets[0].x, m_music_offsets[0].y)));

	for (int i(0); i < 2; i++)
		m_music_sliders[i].Draw(proj.GetTranslated(ae::Vector2f(context_size.x + m_music_offsets[i + 1].x, m_music_offsets[i + 1].y)));
}
void InterfaceLayer::HandleEvent(ae::Event& event) {

	ae::Vector2f context_size = ae::Window.GetContextSize();

	// context events
	event.Dispatch<ae::EventContextClosed>(
		[this](auto& event) -> bool {
			ae::Application.Close();
			return true;
		}
	);

	event.Dispatch<ae::EventContextResized>(
		[this](auto& event) -> bool {
			ae::Camera.SetOrigin(ae::Vector2f(ae::Window.GetContextSize()) / 2.f);
			m_scene->GetContentFramebuffer().Bind();
			m_scene->GetContentFramebuffer().Resize(ae::Window.GetContextSize());
			m_framesprite.SetSize(ae::Window.GetContextSize());
			return false;
		}
	);

	// mouse events
	event.Dispatch<ae::EventMouseButtonPressed>(
		[this, &context_size](auto& event) -> bool {

			// framebuffer filters
			for (int i(0); i < 3; i++)
				if (m_framebuffer_options_boxes[i].HandleEvent(event, context_size + m_framebuffer_options_offsets[i])) {

					if (m_framebuffer_options_boxes[i].IsChecked())
						m_framebuffer_options_choice = i;
					else
						m_framebuffer_options_choice = -1;

					return true;
				}

			// play music 
			else if (m_music_box.HandleEvent(event, ae::Vector2f(context_size.x + m_music_offsets[0].x, m_music_offsets[0].y))) {

				if (m_music_box.IsChecked())
					m_music.Play();
				else
					m_music.Stop();

				return true;
			}

			else if (m_music_sliders[0].HandleEvent(event, ae::Vector2f(context_size.x + m_music_offsets[1].x, m_music_offsets[1].y))) {
				m_music.SetVolume(NORMAL_TO_MUSIC_VOLUME(m_music_sliders[0].GetValue()));
				return true;
			}
			else if (m_music_sliders[1].HandleEvent(event, ae::Vector2f(context_size.x + m_music_offsets[2].x, m_music_offsets[2].y))) {
				m_music.SetPitch(NORMAL_TO_PITCH(m_music_sliders[1].GetValue()));
				return true;
			}

			// local interface
			if (LocalInterface::HandleEventCurrent(event, m_local_interface_position))
				return true;

			return false;
		}
	);
	event.Dispatch<ae::EventMouseMoved>(
		[this, &context_size](auto& event) -> bool {

			// framebuffer filters
			for (int i(0); i < 3; i++)
				m_framebuffer_options_boxes[i].HandleEvent(event, context_size + m_framebuffer_options_offsets[i]);

			// play music
			m_music_box.HandleEvent(event, ae::Vector2f(context_size.x + m_music_offsets[0].x, m_music_offsets[0].y));

			for (int i(0); i < 2; i++)
				m_music_sliders[i].HandleEvent(event, ae::Vector2f(context_size.x + m_music_offsets[i + 1].x, m_music_offsets[i + 1].y));

			m_music.SetVolume(NORMAL_TO_MUSIC_VOLUME(m_music_sliders[0].GetValue()));
			m_music.SetPitch(NORMAL_TO_PITCH(m_music_sliders[1].GetValue()));

			// local interface
			LocalInterface::HandleEventCurrent(event, m_local_interface_position);

			return false;
		}
	);
	event.Dispatch<ae::EventMouseButtonReleased>(
		[this, &context_size](auto& event) -> bool {

			// play music
			for (int i(0); i < 2; i++)
				m_music_sliders[i].HandleEvent(event, ae::Vector2f(context_size.x + m_music_offsets[i + 1].x, m_music_offsets[i + 1].y));

			// local interface
			LocalInterface::HandleEventCurrent(event, m_local_interface_position);

			return false;
		}
	);

	event.Dispatch<ae::EventMouseScrolled>(
		[this](auto& event) -> bool {

			if (UpdateCameraScale(event.GetOffset().y))
				return true;

			return false;
		}
	);
}

void InterfaceLayer::UpdateCamera() const {

	auto& Keyboard = ae::Input.Keyboard;
	auto& Mouse = ae::Input.Mouse;

	if (!Keyboard.IsKeyPressed(ae::KeyboardKey::LeftShift))
		return;

	float delta_time = ae::Application.GetTickTime().GetSeconds();

	// movement
	constexpr float movement_speed = 400.f;
	ae::Vector2f position = ae::Camera.GetPosition();

	if (Keyboard.IsKeyPressed(ae::KeyboardKey::A)) position.x -= movement_speed * delta_time;
	if (Keyboard.IsKeyPressed(ae::KeyboardKey::D)) position.x += movement_speed * delta_time;
	if (Keyboard.IsKeyPressed(ae::KeyboardKey::W)) position.y -= movement_speed * delta_time;
	if (Keyboard.IsKeyPressed(ae::KeyboardKey::S)) position.y += movement_speed * delta_time;

	// update local interface
	if (ae::Camera.GetPosition() != position) {
		bool local_interface_caught = false;

		for (auto& part : m_scene->GetParts())
			if (part.part->GetBounds().IsPointInside(ae::Camera.GetPosition() - part.position))
				if (part.part->GetLocalInterface() != std::string()) {
					LocalInterface::SetCurrent(part.part->GetLocalInterface());
					local_interface_caught = true;
					break;
				}

		if (!local_interface_caught)
			LocalInterface::SetCurrent("");
	}

	ae::Camera.SetPosition(position);

	// rotation
	constexpr float rotation_speed = 40.f;
	float rotation = ae::Camera.GetRotation();

	if (Keyboard.IsKeyPressed(ae::KeyboardKey::Q)) rotation -= rotation_speed * delta_time;
	if (Keyboard.IsKeyPressed(ae::KeyboardKey::E)) rotation += rotation_speed * delta_time;

	ae::Camera.SetRotation(rotation);

	// shear
	constexpr float shear_speed = 1.f;
	ae::Vector2f shear = ae::Camera.GetShear();

	ae::Vector2f shear_offset(
		shear_speed * delta_time,
		shear_speed * delta_time
	);

	if (Keyboard.IsKeyPressed(ae::KeyboardKey::C)) shear -= shear_offset;
	if (Keyboard.IsKeyPressed(ae::KeyboardKey::V)) shear += shear_offset;

	shear.x = std::min(std::max(-0.9f, shear.x), 0.9f);
	shear.y = std::min(std::max(-0.9f, shear.y), 0.9f);

	ae::Camera.SetShear(shear);

	// listener
	ae::Vector2f listener_pos =
		-ae::Camera.GetViewMatrix()
		.TransformPoint(ae::Vector2f(-ae::Camera.GetOrigin()));

	ae::Listener.SetPosition(ae::Vector3f(listener_pos.x, listener_pos.y, SCALE_TO_LISTENER(ae::Camera.GetScale().x)));

}
bool InterfaceLayer::UpdateCameraScale(float scroll_offset) const {

	if (!ae::Input.Keyboard.IsKeyPressed(ae::KeyboardKey::LeftShift))
		return false;

	float delta_time = ae::Application.GetTickTime().GetSeconds();
	constexpr float scale_speed = 40.f;

	ae::Vector2f scale(
		ae::Camera.GetScale().x * (1.f + scale_speed * scroll_offset * delta_time),
		ae::Camera.GetScale().y * (1.f + scale_speed * scroll_offset * delta_time)
	);

	scale.x = std::min(std::max(0.3f, scale.x), 3.f);
	scale.y = std::min(std::max(0.3f, scale.y), 3.f);

	ae::Camera.SetScale(scale);

	// listener
	ae::Vector2f listener_pos =
		-ae::Camera.GetViewMatrix()
		.TransformPoint(ae::Vector2f(-ae::Camera.GetOrigin()));

	ae::Listener.SetPosition(ae::Vector3f(listener_pos.x, listener_pos.y, SCALE_TO_LISTENER(scale.x)));

	return true;
}
void InterfaceLayer::UpdateInfo() {

	static ae::Clock update_clock;

	if (update_clock.GetElapsedTime().GetSeconds() > 0.1f) {

		int fps = 1.f / ae::Application.GetTickTime().GetSeconds();
		int runtime = ae::Application.GetRunTime().GetSeconds();

		std::u32string info_string =
			U"FPS = " + ae::utf::ConvertToUtf32(std::to_string(fps)) + U"\n" +
			U"RunTime = " + ae::utf::ConvertToUtf32(std::to_string(runtime)) + U"s"
			;

		m_info_text.SetString(std::move(info_string));

		update_clock.Restart();
	}
}