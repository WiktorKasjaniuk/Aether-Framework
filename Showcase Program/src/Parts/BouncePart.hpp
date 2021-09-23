
#pragma once

#include <Aether.hpp>
#include "Part.hpp"
#include "../GUI/GUI.hpp"
#include "../LocalInterface.hpp"

class BoundsComp : public ae::Component {
public:
	BoundsComp(const ae::FloatRect& bounds, const ae::Vector2f& parrent_model)
		: ae::Component(), m_position(bounds.left, bounds.top), m_size(bounds.width, bounds.height), m_parrent_model(parrent_model) {}

	void SetPosition(const ae::Vector2f& position) { m_position = position; }
	void SetSize(const ae::Vector2f& size) { m_size = size; }

	const ae::Vector2f& GetPosition() const { return m_position; }
	const ae::Vector2f& GetSize() const { return m_size; }
	const ae::Vector2f& GetParrentModel() const { return m_parrent_model; }

private:
	ae::Vector2f m_position;
	ae::Vector2f m_size;
	const ae::Vector2f m_parrent_model;
};
class PhysicsComp : public ae::Component {
public:
	PhysicsComp(float angle_degrees, float velocity);

	void Update(const ae::FloatRect& box_bounds, float deltatime);
	void SetVelocity(float velocity) { m_velocity = velocity; }

private:
	float m_direction, m_velocity;

	bool m_can_change_dir[2] = { true, true };
};
class AudioComp : public ae::Component {
public:
	AudioComp(const ae::SoundBuffer& sound_buffer, float volume = 1.f);
	void Play(const ae::Vector2f& position);

private:
	ae::Sound m_sound;
};
class AnimationComp : public ae::Component {
public:
	AnimationComp(const std::vector<ae::IntRect>& frames, float speed = 1.f, size_t current_frame = 0)
		: ae::Component(), m_frames(frames), m_current_frame(current_frame), m_speed(speed) {}

	void Update(float deltatime);
	const ae::IntRect& GetCurrentTexCoords() const { return m_frames[m_current_frame]; }

private:
	std::vector<ae::IntRect> m_frames;
	size_t m_current_frame;
	float m_speed, m_total = 0.f;
};
class ParticleComp : public ae::Component {
public:
	ParticleComp(ae::BatchSpriteRenderer& renderer, float lifetime, float speed_multiplier);
	~ParticleComp();

	void Update(float deltatime);
	void Deconstruct();

private:
	float m_lifetime;
	const float m_max_lifetime, m_speed_multiplier;
	ae::BatchSprite* m_sprite;
};

//

class BouncePart;
class BounceInterface : public LocalInterface {
	friend class BouncePart;

public:
	BounceInterface();

	virtual void Draw(const ae::Matrix3x3& transform) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Vector2f& position) override;
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Vector2f& position) override;

private:
	ae::Text m_title, m_note;
	GUIChooseBox m_comp_boxes[5];
};
class BouncePart : public Part {
public:
	BouncePart();
	~BouncePart();

	virtual void Draw(const ae::Matrix3x3& transform) override;
	virtual void Update(float deltatime) override;

	virtual bool HandleEvent(ae::EventMouseButtonPressed& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;
	virtual void HandleEvent(ae::EventMouseMoved& event, const ae::Matrix3x3& view, const ae::Vector2f& model) override;

private:
	BounceInterface* m_interface;

	ae::Text m_description;
	ae::Shape m_background;
	ae::BatchSpriteRenderer m_renderer;
};