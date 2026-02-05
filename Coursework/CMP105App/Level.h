#pragma once

#include "Framework/BaseLevel.h"
#include "Framework/GameObject.h"
#include "Pig.h"
#include "Sheep.h"
#include "Framework/Collision.h"
#include <memory>
#include <vector>


struct SpeedText
{
	sf::Text text;

	// SFML 3: text MUST have a font at construction
	SpeedText(const sf::Font& font) : text(font)
	{
		text.setCharacterSize(20);
		text.setFillColor(sf::Color::White);
		text.setPosition({ 10.f, 10.f });
		text.setString("Speed: 0");
	}
};

class Level : BaseLevel {
public:
	Level(sf::RenderWindow& window, Input& input);
	~Level();

	void handleInput(float dt) override;
	void update(float dt);
	void render();

private:
	// Default functions for rendering to the screen.

	// Default variables for level class.
	bool m_gameOver;

	sf::Texture m_backgroundTexture;
	GameObject m_background;

	// Here are our nice animals
	sf::Texture m_pigTexture;
	sf::Texture m_sheepTexture;
	std::vector<Pig*> m_pigPointers;
	Sheep m_sheep;

	// camera shake
	float m_shakeTimer = 0.f;
	const float SHAKE_TIME = 0.2f;
	const float SHAKE_INTENSITY = 3.f; // tweakable

	// UI elements
	sf::Font m_font;
	// We use a pointer because sf::Text needs the font to exist before it is created
	std::unique_ptr<SpeedText> m_speedText;

	float m_currentSpeed = 0.f; // store current speed

};