#pragma once
#include "Framework/GameObject.h"
#include "Framework/Animation.h"

class Sheep :
	public GameObject
{

public:
	Sheep();
	~Sheep();

	void handleInput(float dt) override;
	void update(float dt) override;

	void setWorldSize(sf::Vector2f worldSize);

	void collisionResponse(GameObject& collider) override;


private:
	const float ACCELERATION = 500.0f;   // rate of acceleration
	const float DRAG_COEFFICIENT = 0.80f; // higher = more friction (mud)
	
	sf::Vector2f m_acceleration;

	sf::Vector2f m_worldSize;

	const float COEFF_OF_RESTITUTION = 0.8f; // energy retained after collision

	void checkWallAndBounce();

	// collision
	const float SLOW_COLLISION_SPEED_SQ = 2500.f; // 50 units/sec
	const float SLOW_PUSH_FACTOR = 0.5f;
	const float FAST_PUSH_FACTOR = 0.05f;

	Animation m_walkDown;
	Animation m_walkUp;
	Animation m_walkUpRight;
	Animation m_walkRight;
	Animation m_walkDownRight;

	Animation* m_currentAnimation;
};

