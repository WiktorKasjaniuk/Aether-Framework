
#include "BouncePart.hpp"

#include <random>

constexpr float c_box_padding = 30.f, c_box_offset = 80.f;

PhysicsComp::PhysicsComp(float angle_degrees, float velocity)
	: ae::Component(), m_direction(angle_degrees), m_velocity(velocity) {}

void PhysicsComp::Update(const ae::FloatRect& box_bounds, float deltatime) {

	ae::Entity& entity = *GetEntity();
	BoundsComp* bounds = entity.HasComponent<BoundsComp>() ? &entity.GetComponent<BoundsComp>() : nullptr;

	if (!bounds)
		return;

	// move
	const ae::Vector2f& position = bounds->GetPosition();
	const ae::Vector2f& size = bounds->GetSize();

	ae::Vector2f movement(
		sin(m_direction * 3.14159f / 180.f) * m_velocity * deltatime,
		cos(m_direction * 3.14159f / 180.f) * m_velocity * deltatime
	);
	bounds->SetPosition(position + movement);

	// check if should bounce
	bool bounced = false;

	if (box_bounds.top > position.y || box_bounds.top + box_bounds.height < position.y + size.y) {
		if (m_can_change_dir[1] == true) {
			m_can_change_dir[1] = false;
			m_direction = fmod(180.f - m_direction, 360.f);

			float pos_y = (box_bounds.top > position.y) ? box_bounds.top : (box_bounds.top + box_bounds.height - size.y);
			bounds->SetPosition(ae::Vector2f(bounds->GetPosition().x, pos_y));

			bounced = true;
		}
	}
	else m_can_change_dir[1] = true;

	if (box_bounds.left > position.x || box_bounds.left + box_bounds.width < position.x + size.x) {
		if (m_can_change_dir[0] == true) {
			m_can_change_dir[0] = false;
			m_direction = -m_direction;

			float pos_x = (box_bounds.left > position.x) ? box_bounds.left : (box_bounds.left + box_bounds.width - size.x);
			bounds->SetPosition(ae::Vector2f(pos_x, bounds->GetPosition().y));

			bounced = true;
		}
	}
	else m_can_change_dir[0] = true;

	// generate sound if needed
	if (bounced && GetEntity()->HasComponent<AudioComp>())
		GetEntity()->GetComponent<AudioComp>().Play(bounds->GetParrentModel() + position + size * 0.5f);
}

AudioComp::AudioComp(const ae::SoundBuffer& sound_buffer, float volume)
	: ae::Component()
{
	m_sound.SetSoundBuffer(sound_buffer);
	m_sound.SetVolume(volume);
}
void AudioComp::Play(const ae::Vector2f& position) {
	m_sound.SetPosition(ae::Vector3f(position.x, position.y, 0.f));
	m_sound.Play();
}

void AnimationComp::Update(float deltatime) {
	m_total += m_speed * deltatime;
	m_total = fmod(m_total, m_frames.size());
	m_current_frame = m_total;
}

ParticleComp::ParticleComp(ae::BatchSpriteRenderer& renderer, float lifetime, float speed_multiplier)
	: ae::Component(), m_lifetime(lifetime), m_max_lifetime(lifetime), m_speed_multiplier(speed_multiplier)
{
	m_sprite = &renderer.CreateBack();
	m_sprite->SetTextureRect(ae::IntRect(0, 0, 100, 100));
	m_sprite->SetSize(ae::Vector2f(100.f, 100.f));
}
ParticleComp::~ParticleComp() {
	Deconstruct();
}
void ParticleComp::Deconstruct() {
	if (m_sprite)
		m_sprite->GetRenderer().Destroy(m_sprite->GetIndex());

	m_sprite = nullptr;
}

void ParticleComp::Update(float deltatime) {

	ae::Entity& entity = *GetEntity();
	BoundsComp* bounds = entity.HasComponent<BoundsComp>() ? &entity.GetComponent<BoundsComp>() : nullptr;
	PhysicsComp* physics = entity.HasComponent<PhysicsComp>() ? &entity.GetComponent<PhysicsComp>() : nullptr;
	AnimationComp* animation = entity.HasComponent<AnimationComp>() ? &entity.GetComponent<AnimationComp>() : nullptr;

	if (!bounds)
		return;

	// update position
	m_sprite->SetTransform(ae::Matrix3x3::Identity.GetTranslated(bounds->GetPosition()));

	// update texture coords
	if (animation)
		m_sprite->SetTextureRect(animation->GetCurrentTexCoords());

	// age
	const float age = m_lifetime / m_max_lifetime;
	m_lifetime -= deltatime;
	
	if (m_lifetime <= 0.f)
		entity.Kill();

	std::uint8_t alpha = age * 255.f;
	ae::Color c = ae::Color(255, 255, 255, alpha);
	m_sprite->SetColors(c, c, c, c);

	if (physics)
		physics->SetVelocity(m_speed_multiplier * 500.f * age);
}

//

BounceInterface::BounceInterface() {

	// title
	m_title.SetCharSize(20);
	m_title.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_title.SetString(U"== Entity Bouncer ==");

	// note
	m_note.SetCharSize(20);
	m_note.SetColor(ae::Color(200, 200, 200, 255));
	m_note.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_note.SetString(U"Create entity - Left Mouse Button (on the box)\nPlay Sound - Right / Middle Mouse Button (on a tile)\n\nECS example choose components for the next entity:");

	// components
	m_comp_boxes[0].SetShape(GUIChooseBox::Shape::Square);
	m_comp_boxes[0].SetString(U"BoundsComp");
	m_comp_boxes[0].AllowManualUncheck(false);
	m_comp_boxes[0].SetChecked(true);

	m_comp_boxes[1].SetShape(GUIChooseBox::Shape::Square);
	m_comp_boxes[1].SetString(U"ParticleComp");
	m_comp_boxes[1].AllowManualUncheck(false);
	m_comp_boxes[1].SetChecked(true);

	m_comp_boxes[2].SetShape(GUIChooseBox::Shape::Square);
	m_comp_boxes[2].SetString(U"PhysicsComp");
	m_comp_boxes[2].SetChecked(true);

	m_comp_boxes[3].SetShape(GUIChooseBox::Shape::Square);
	m_comp_boxes[3].SetString(U"AudioComp");
	m_comp_boxes[3].SetChecked(true);

	m_comp_boxes[4].SetShape(GUIChooseBox::Shape::Square);
	m_comp_boxes[4].SetString(U"AnimationComp");
	m_comp_boxes[4].SetChecked(true);
}
void BounceInterface::Draw(const ae::Matrix3x3& transform) {
	m_title.Draw(transform.GetTranslated(ae::Vector2f(0.f, -20.f)));
	m_note.Draw(transform);

	for (int i(0); i < 5; i++)
		m_comp_boxes[i].Draw(transform.GetTranslated(ae::Vector2f(0.f, c_box_offset + i * c_box_padding)));
}
bool BounceInterface::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) {

	for (int i(0); i < 5; i++)
		if (m_comp_boxes[i].HandleEvent(event, ae::Vector2f(position.x, position.y + c_box_offset + i * c_box_padding)))
			return true;

	return false;
}
void BounceInterface::HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) {

	for (int i(0); i < 5; i++)
		m_comp_boxes[i].HandleEvent(event, ae::Vector2f(position.x, position.y + c_box_offset + i * c_box_padding));
}

BouncePart::BouncePart() 
	: Part(ae::FloatRect(-100.f, -100.f, 1000.f, 1000.f), "bounce")
{
	// description
	m_description.SetCharSize(50);
	m_description.SetFont(ae::AssetManager.GetFont("urbanist"));
	m_description.SetString(U"Entity Component System\n+ batch rendering elements!");

	// background
	m_background.SetColor(ae::Color(30, 140, 255, 100));
	m_background.SetIndices({ 0,1,2, 2,3,0 });
	m_background.SetVertices({
		ae::VertexPos(0.f, 0.f), ae::VertexPos(600.f, 0.f),
		ae::VertexPos(600.f, 600.f), ae::VertexPos(0.f, 600.f)
	});

	// tiles
	m_renderer.SetTexture(ae::AssetManager.GetTexture("bounce_tile"));

	// local interface
	m_interface = LocalInterface::Create<BounceInterface>("bounce");
}

BouncePart::~BouncePart() {

	// safety destroy all particles, so they won't try do destroy non existent instanced sprites
	ae::EntityManager.ViewComponents<ParticleComp>(
		[](ParticleComp& comp) {
			comp.Deconstruct();
		}
	);

	LocalInterface::Delete("bounce");
}

void BouncePart::Draw(const ae::Matrix3x3& transform) {
	m_description.Draw(transform.GetTranslated(ae::Vector2f(0.f, -90.f)));
	m_background.Draw(transform);
	m_renderer.Draw(transform);
}
void BouncePart::Update(float deltatime) {

	ae::EntityManager.ViewComponents<ParticleComp>(
		[&deltatime](ParticleComp& comp) {
			if (comp.GetEntity()->IsAlive())
				comp.Update(deltatime);
		}
	);
	
	ae::FloatRect box_bounds(m_background.GetVertices()[0].position, m_background.GetVertices()[2].position);
	ae::EntityManager.ViewComponents<PhysicsComp>(
		[&box_bounds, &deltatime](PhysicsComp& comp) {
			if (comp.GetEntity()->IsAlive())
				comp.Update(box_bounds, deltatime);
		}
	);

	ae::EntityManager.ViewComponents<AnimationComp>(
		[&box_bounds, &deltatime](AnimationComp& comp) {
			if (comp.GetEntity()->IsAlive())
				comp.Update(deltatime);
		}
	);
}
bool BouncePart::HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

	// transform mouse to world coordinates
	ae::Vector2f fixed_mouse = view.GetInverse().TransformPoint(ae::Input.Mouse.GetPosition());
	ae::FloatRect box_bounds(model + m_background.GetVertices()[0].position, m_background.GetVertices()[2].position);

	// create
	if (box_bounds.IsPointInside(fixed_mouse)) {
		if (event.GetButton() == ae::MouseButton::Left) {
			std::default_random_engine engine(ae::Application.GetRunTime().GetMicroseconds());
			using Gen = std::uniform_real_distribution<float>;

			ae::Entity& tile = ae::EntityManager.CreateEntity();

			// bounds
			ae::Vector2f size(100.f, 100.f);
			const ae::Vector2f& box_size = m_background.GetVertices()[2].position;
			ae::Vector2f pos = fixed_mouse - model - size * 0.5f;
			pos.x = std::min(std::max(pos.x, size.x * 0.5f - 50.f), box_size.x - size.x * 0.5f - 50.f);
			pos.y = std::min(std::max(pos.y, size.y * 0.5f - 50.f), box_size.y - size.y * 0.5f - 50.f);

			tile.AddComponent<BoundsComp>(ae::FloatRect(pos, size), model);

			// particle
			if (m_interface->m_comp_boxes[1].IsChecked())
				tile.AddComponent<ParticleComp>(m_renderer, Gen(5.f, 15.f)(engine), Gen(0.6f, 1.2f)(engine));

			// physics
			if (m_interface->m_comp_boxes[2].IsChecked())
				tile.AddComponent<PhysicsComp>(Gen(0.f, 360.f)(engine), 0.f);

			// audio
			if (m_interface->m_comp_boxes[3].IsChecked())
				tile.AddComponent<AudioComp>(ae::AssetManager.GetSoundBuffer("bounce_tile_bounce"));

			// animation
			if (m_interface->m_comp_boxes[4].IsChecked())
				tile.AddComponent<AnimationComp>(
					std::vector<ae::IntRect> { 
						ae::IntRect(0, 0, 100, 100), ae::IntRect(100, 0, 100, 100), ae::IntRect(200, 0, 100, 100), ae::IntRect(300, 0, 100, 100) 
					}, 2.f
				);

			return true;
		}

		// play sound
		else {
			auto p_it = ae::EntityManager.GetBeginEndIterators<BoundsComp>();
			auto begin = p_it.first;
			auto end = p_it.second;

			for (auto comp_it(begin); comp_it != end; comp_it++) {
				BoundsComp& comp = static_cast<BoundsComp&>(**comp_it);

				if (comp.GetEntity()->IsAlive()) {
					ae::Entity& entity = *comp.GetEntity();

					ae::Vector2f pos = model + comp.GetPosition();
					const ae::Vector2f& size = comp.GetSize();

					if (
						fixed_mouse.x >= pos.x && fixed_mouse.x < pos.x + size.x &&
						fixed_mouse.y >= pos.y && fixed_mouse.y < pos.y + size.y
					) 
					{
						if (entity.HasComponent<AudioComp>())
							entity.GetComponent<AudioComp>().Play(pos + size * 0.5f);

						break;
					}
				}
			}
		}
		return true;
	}

	return false;
}
void BouncePart::HandleEvent(ae::EventMouseMoved& event, const ae::Matrix3x3& view, const ae::Vector2f& model) {

}