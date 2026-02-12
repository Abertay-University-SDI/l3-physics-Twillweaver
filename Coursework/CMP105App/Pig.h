#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"

class Pig : public GameObject
{
public:

	Pig(sf::Vector2f worldSize);
	~Pig();

	void update(float dt) override;
	void setRandomDirection();

	void collisionResponse(GameObject& collider) override;


private:
	// direction used for flipping animation on top/bottom
	enum class Direction { UP, UP_RIGHT, RIGHT, DOWN_RIGHT, DOWN, DOWN_LEFT, LEFT, UP_LEFT, NONE };
	Direction m_direction = Direction::DOWN;
	const float APPROX_ONE_OVER_ROOT_TWO = 0.70710678f;	// 1 / sqrt(2)
	float m_decisionTimer = 0.0f;
	bool m_isGrazing = false;
	sf::Vector2f m_worldSize;	// pass as param to constructor

	Animation m_walkDown;
	Animation m_walkUp;
	Animation m_walkUpRight;
	Animation m_walkRight;
	Animation m_walkDownRight;
	Animation* m_currentAnimation;

	const float WALK_SPEED = 100.0f;
	const float PIG_THINKING_TIME = 2.0f; // Time between pig event. 2s as default
	const int GRAZE_CHANCE = 40; // % chance of stopping to graze. 40 as default

	float m_hitTimer = 0.0f;
	const float FLASH_DURATION = 0.2f;

};