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


void Sheep::update(float dt)
{
	// v = u + at
	m_velocity += m_acceleration * dt;

	// apply drag (friction)
	m_velocity -= m_velocity * DRAG_COEFFICIENT * dt;

	// s = vt
	move(m_velocity * dt);

	// wall collisions
	checkWallAndBounce();

	sf::Vector2f pos = getPosition();

	if (pos.x + getSize().x > m_worldSize.x)
	{
		std::cout << "Sheep left right boundary\n";
	}

	if (pos.y + getSize().y > m_worldSize.y)
	{
		std::cout << "Sheep left bottom boundary\n";
	}


	// update animation
	if (std::abs(m_velocity.x) > 1.f || std::abs(m_velocity.y) > 1.f)
	{
		m_currentAnimation->animate(dt);
		setTextureRect(m_currentAnimation->getCurrentFrame());
	}
}
