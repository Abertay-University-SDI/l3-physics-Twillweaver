#include "Sheep.h"
#include <iostream>

Sheep::Sheep()
{
	// initialise animations
	for (int i = 0; i < 4; i++)
		m_walkDown.addFrame({ { 64 * i, 0 }, { 64, 64 } });
	m_walkDown.setLooping(true);
	m_walkDown.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkUp.addFrame({ { (64 * (i + 4)), 0 }, { 64, 64 } });
	m_walkUp.setLooping(true);
	m_walkUp.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkUpRight.addFrame({ { 64 * i, 64 }, { 64, 64 } });
	m_walkUpRight.setLooping(true);
	m_walkUpRight.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkRight.addFrame({ { (64 * (i + 4)), 64 }, { 64, 64 } });
	m_walkRight.setLooping(true);
	m_walkRight.setFrameSpeed(0.25f);

	for (int i = 0; i < 4; i++)
		m_walkDownRight.addFrame({ { 64 * i, 128 }, { 64, 64 } });
	m_walkDownRight.setLooping(true);
	m_walkDownRight.setFrameSpeed(0.25f);

	// the next 4 animations go clockwise from Up through Right to Down.

	m_currentAnimation = &m_walkDown;
	setTextureRect(m_currentAnimation->getCurrentFrame());

	setCollider(true);
	setCollisionBox(2.f, 2.f, 60.f, 60.f);

}

Sheep::~Sheep()
{
}

void Sheep::setWorldSize(sf::Vector2f worldSize)
{
	m_worldSize = worldSize;
}

void Sheep::handleInput(float dt)
{
	sf::Vector2f inputDir = { 0.f, 0.f };

	if (m_input->isKeyDown(sf::Keyboard::Scancode::W))
		inputDir.y -= 1.f;
	if (m_input->isKeyDown(sf::Keyboard::Scancode::S))
		inputDir.y += 1.f;
	if (m_input->isKeyDown(sf::Keyboard::Scancode::A))
		inputDir.x -= 1.f;
	if (m_input->isKeyDown(sf::Keyboard::Scancode::D))
		inputDir.x += 1.f;

	m_acceleration = inputDir * ACCELERATION;
}
void Sheep::checkWallAndBounce()
{
	sf::Vector2f pos = getPosition();
	sf::Vector2f size = getSize();

	// RIGHT wall
	if (pos.x + size.x > m_worldSize.x && m_velocity.x > 0.f)
	{
		pos.x = m_worldSize.x - size.x;   // snap inside
		m_velocity.x = -m_velocity.x;
		m_velocity *= COEFF_OF_RESTITUTION;
		setPosition(pos);
	}

	// LEFT wall
	if (pos.x < 0.f && m_velocity.x < 0.f)
	{
		pos.x = 0.f;
		m_velocity.x = -m_velocity.x;
		m_velocity *= COEFF_OF_RESTITUTION;
		setPosition(pos);
	}

	// BOTTOM wall
	if (pos.y + size.y > m_worldSize.y && m_velocity.y > 0.f)
	{
		pos.y = m_worldSize.y - size.y;
		m_velocity.y = -m_velocity.y;
		m_velocity *= COEFF_OF_RESTITUTION;
		setPosition(pos);
	}

	// TOP wall
	if (pos.y < 0.f && m_velocity.y < 0.f)
	{
		pos.y = 0.f;
		m_velocity.y = -m_velocity.y;
		m_velocity *= COEFF_OF_RESTITUTION;
		setPosition(pos);
	}
}

void Sheep::collisionResponse(GameObject& collider)
{
	// Lazy bounce: invert velocity
	m_velocity = -m_velocity;

	// Lose energy on impact
	m_velocity *= COEFF_OF_RESTITUTION;

	// measure the speed after a bounce
	float speedSq = m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y;

	// fush distance is influenced by the speed of the collision
	float pushFactor;

	// push threshold (50 units/sec)
	if (speedSq < SLOW_COLLISION_SPEED_SQ)
	{
		// slow collison = big shove
		pushFactor = SLOW_PUSH_FACTOR;
	}
	else
	{
		// fast collsion = slow shove
		pushFactor = FAST_PUSH_FACTOR;
	}
	// push the sheep clear out of the way
	move(m_velocity * pushFactor);
}


void Sheep::update(float dt)
{
	// 1. Physics: Velocity, Drag, and Move
	m_velocity += m_acceleration * dt;
	m_velocity -= m_velocity * DRAG_COEFFICIENT * dt;

	float speed = std::sqrt(m_velocity.x * m_velocity.x + m_velocity.y * m_velocity.y);
	if (speed > MAX_SPEED) {
		m_velocity = (m_velocity / speed) * MAX_SPEED;
	}

	move(m_velocity * dt);
	checkWallAndBounce();

	// 2. Directional Animation Switching
	// We only update the animation if the sheep is actually moving
	if (speed > 10.f)
	{
		// Calculate angle in degrees: Right = 0, Down = 90, Left = 180, Up = -90
		float angle = std::atan2(m_velocity.y, m_velocity.x) * 180.f / 3.14159f;

		if (angle > -22.5f && angle <= 22.5f) {
			m_currentAnimation = &m_walkRight;
			m_currentAnimation->setFlipped(false);
		}
		else if (angle > 22.5f && angle <= 67.5f) {
			m_currentAnimation = &m_walkDownRight;
			m_currentAnimation->setFlipped(false);
		}
		else if (angle > 67.5f && angle <= 112.5f) {
			m_currentAnimation = &m_walkDown;
			m_currentAnimation->setFlipped(false);
		}
		else if (angle > 112.5f && angle <= 157.5f) {
			m_currentAnimation = &m_walkDownRight;
			m_currentAnimation->setFlipped(true); // Left-Down mirror
		}
		else if (angle > 157.5f || angle <= -157.5f) {
			m_currentAnimation = &m_walkRight;
			m_currentAnimation->setFlipped(true); // Left mirror
		}
		else if (angle > -157.5f && angle <= -112.5f) {
			m_currentAnimation = &m_walkUpRight;
			m_currentAnimation->setFlipped(true); // Left-Up mirror
		}
		else if (angle > -112.5f && angle <= -67.5f) {
			m_currentAnimation = &m_walkUp;
			m_currentAnimation->setFlipped(false);
		}
		else if (angle > -67.5f && angle <= -22.5f) {
			m_currentAnimation = &m_walkUpRight;
			m_currentAnimation->setFlipped(false);
		}

		// 3. Dynamic Animation Speed
		// Logic: Faster sheep = lower 'frameSpeed' value (faster cycling)
		// Base delay is 0.25s. At max speed, we want it closer to 0.08s.
		float speedRatio = speed / MAX_SPEED;
		float dynamicFrameSpeed = 0.3f - (speedRatio * 0.22f);
		m_currentAnimation->setFrameSpeed(dynamicFrameSpeed);

		// 4. Finalize Animation Frame
		m_currentAnimation->animate(dt);
		setTextureRect(m_currentAnimation->getCurrentFrame());
	}
}
