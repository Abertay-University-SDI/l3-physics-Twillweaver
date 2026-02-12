#include "Pig.h"
#include <random>
#include <iostream>

Pig::Pig(sf::Vector2f worldSize)
{
	// initialise animations
	for (int i = 0; i < 4; i++)
		m_walkDown.addFrame({ { 64 * i, 0 }, { 64, 64 } });
	m_walkDown.setLooping(true);
	m_walkDown.setFrameSpeed(0.1f);

	for (int i = 0; i < 4; i++)
		m_walkUp.addFrame({ { (64 * (i + 4)), 0 }, { 64, 64 } });
	m_walkUp.setLooping(true);
	m_walkUp.setFrameSpeed(0.1f);

	for (int i = 0; i < 4; i++)
		m_walkUpRight.addFrame({ { 64 * i, 64 }, { 64, 64 } });
	m_walkUpRight.setLooping(true);
	m_walkUpRight.setFrameSpeed(0.1f);

	for (int i = 0; i < 4; i++)
		m_walkRight.addFrame({ { (64 * (i + 4)), 64 }, { 64, 64 } });
	m_walkRight.setLooping(true);
	m_walkRight.setFrameSpeed(0.1f);

	for (int i = 0; i < 4; i++)
		m_walkDownRight.addFrame({ { 64 * i, 128 }, { 64, 64 } });
	m_walkDownRight.setLooping(true);
	m_walkDownRight.setFrameSpeed(0.1f);

    m_worldSize = worldSize;

	m_currentAnimation = &m_walkDown;
	setTextureRect(m_currentAnimation->getCurrentFrame());

	setCollider(true);
	setCollisionBox(2.f, 2.f, 60.f, 60.f);

}

Pig::~Pig()
{
}

void Pig::collisionResponse(GameObject& collider)
{
	// Trigger the flash effect
	m_hitTimer = FLASH_DURATION;

	// Set hit colour
	this->setFillColor(sf::Color(255, 100, 100, 255));

	std::cout << "Sheep smashed into a pig!\n";
}


void Pig::setRandomDirection()
{

	// Use the Mersenne Twister engine to generate a random number [0, 7]
	std::random_device rd;
	std::uniform_int_distribution<int> distribution(0, 7);
	std::mt19937 engine(rd());

	switch (distribution(engine))
	{
	case 0:
		m_direction = Direction::UP_LEFT;
		m_currentAnimation = &m_walkUpRight;
		m_currentAnimation->setFlipped(true);
		setVelocity({
			-WALK_SPEED * APPROX_ONE_OVER_ROOT_TWO,
			-WALK_SPEED * APPROX_ONE_OVER_ROOT_TWO
		});
		break;
	
	case 1:
		m_direction = Direction::UP;
		m_currentAnimation = &m_walkUp;
		setVelocity({
			0,
			-WALK_SPEED
		});
		break;
	
	case 2:
		m_direction = Direction::UP_RIGHT;
		m_currentAnimation = &m_walkUpRight;
		m_currentAnimation->setFlipped(false);
		setVelocity({
			WALK_SPEED * APPROX_ONE_OVER_ROOT_TWO,
			-WALK_SPEED * APPROX_ONE_OVER_ROOT_TWO
		});
		break;

	case 3:
		m_direction = Direction::RIGHT;
		m_currentAnimation = &m_walkRight;
		m_currentAnimation->setFlipped(false);
		setVelocity({
			WALK_SPEED,
			0
		});
		break;

	case 4:
		m_direction = Direction::DOWN_RIGHT;
		m_currentAnimation = &m_walkDownRight;
		m_currentAnimation->setFlipped(false);
		setVelocity({
			WALK_SPEED * APPROX_ONE_OVER_ROOT_TWO,
			WALK_SPEED * APPROX_ONE_OVER_ROOT_TWO
		});
		break;

	case 5:
		m_direction = Direction::DOWN;
		m_currentAnimation = &m_walkDown;
		setVelocity({
			0,
			WALK_SPEED
		});
		break;

	case 6:
		m_direction = Direction::DOWN_LEFT;
		m_currentAnimation = &m_walkDownRight;
		m_currentAnimation->setFlipped(true);
		setVelocity({
			-WALK_SPEED* APPROX_ONE_OVER_ROOT_TWO,
			WALK_SPEED* APPROX_ONE_OVER_ROOT_TWO
		});
		break;

	case 7:
		m_direction = Direction::LEFT;
		m_currentAnimation = &m_walkRight;
		m_currentAnimation->setFlipped(true);
		setVelocity({
			-WALK_SPEED,
			0
		});
		break;
	}
}

void Pig::update(float dt)
{
	// Handle hit flash timer
	if (m_hitTimer > 0.0f)
	{
		m_hitTimer -= dt;
		if (m_hitTimer <= 0.0f)
		{
			// Reset to fully opaque/normal color
			this->setFillColor(sf::Color::White);
		}
	}

    // tick down
    m_decisionTimer -= dt;

    if (m_decisionTimer < 0.0f)
    {
        // Reset timer (pick a random time between 1.0s and 3.0s)
        m_decisionTimer = PIG_THINKING_TIME;

        // 40% chance to stop and graze, 60% chance to walk
        if ((rand() % 100) < GRAZE_CHANCE)
        {
            m_isGrazing = true;
            m_velocity = { 0.f, 0.f };
        }
        else
        {
            m_isGrazing = false;
			setRandomDirection();
		}
    }

	if (m_isGrazing) return;	// no animation, movement or boundary checks on static lads
	
	move(m_velocity * dt);
	m_currentAnimation->animate(dt);
    setTextureRect(m_currentAnimation->getCurrentFrame());

    // Bouncey Pigs
    sf::Vector2f pos = getPosition();
	if ((pos.x < 0 && m_velocity.x < 0) || (pos.x + getSize().x > m_worldSize.x && m_velocity.x > 0))
	{
		// bounce on the sides
		m_velocity.x = -m_velocity.x;
		m_currentAnimation->setFlipped(!m_currentAnimation->getFlipped());
		m_direction = m_direction == Direction::RIGHT ? Direction::LEFT : Direction::RIGHT;
	}
	if ((pos.y < 0 && m_velocity.y < 0) || (pos.y + getSize().y > m_worldSize.y && m_velocity.y > 0))
	{
		// bounce top & bottom
		m_velocity.y = -m_velocity.y;
		switch (m_direction)
		{
		case Direction::UP:
			m_direction = Direction::DOWN;
			m_currentAnimation = &m_walkDown;
			break;
		case Direction::DOWN:
			m_direction = Direction::UP;
			m_currentAnimation = &m_walkUp;
			break;
		case Direction::DOWN_LEFT:
			m_direction = Direction::UP_LEFT;
			m_currentAnimation = &m_walkUpRight;
			m_currentAnimation->setFlipped(true);
			break;
		case Direction::DOWN_RIGHT:
			m_direction = Direction::UP_RIGHT;
			m_currentAnimation = &m_walkUpRight;
			m_currentAnimation->setFlipped(false);
			break;
		case Direction::UP_RIGHT:
			m_direction = Direction::DOWN_RIGHT;
			m_currentAnimation = &m_walkDownRight;
			m_currentAnimation->setFlipped(false);
			break;
		case Direction::UP_LEFT:
			m_direction = Direction::DOWN_LEFT;
			m_currentAnimation = &m_walkDownRight;
			m_currentAnimation->setFlipped(true);
			break;
		// Note: LEFT and RIGHT not handled as you can't hit top/bottom 
		// .. if you aren't going up or down. 
		}
	}
	
}