#include "Level.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <filesystem>

Level::Level(sf::RenderWindow& hwnd, Input& in) :
	BaseLevel(hwnd, in)
{
	// 1. Load Font First (Crucial for SFML 3)
	if (!m_font.openFromFile("font/arial.ttf")) {
		std::cerr << "Failed to open font\n";
	}

	// 2. Initialize SpeedText using the loaded font
	m_speedText = std::make_unique<SpeedText>(m_font);

	// setup background
	float background_size = 1024.f;

	// background
	if (!m_backgroundTexture.loadFromFile("gfx/field.png"))
		std::cerr << "Yikes, no field\n";
	m_background.setTexture(&m_backgroundTexture);
	m_background.setSize({ background_size, background_size});

	// Setup Sheep.
	if (!m_sheepTexture.loadFromFile("gfx/sheep_sheet.png"))
		std::cerr << "No sheep, terrible news\n";
	m_sheep.setInput(&m_input);
	m_sheep.setTexture(&m_sheepTexture);
	m_sheep.setPosition({ background_size / 2.f, background_size / 2.f });
	m_sheep.setSize({ 64,64 });

	m_sheep.setWorldSize({ background_size, background_size });

	// Setup pigs.
	std::vector<sf::Vector2f> pig_locations = {	// top corners and bottom middle
		{background_size * .33f, background_size * .25f },
		{background_size * .67f, background_size * .25f },
		{background_size * .5f, background_size * .75f }
	};
	if (!m_pigTexture.loadFromFile("gfx/pig_sheet.png"))
		std::cerr << "No pigs. Nightmare\n";
	for (int i = 0; i < 3; i++)
	{
		Pig* new_pig = new Pig({ background_size, background_size });
		new_pig->setTexture(&m_pigTexture);
		new_pig->setSize({ 64, 64 });
		new_pig->setPosition(pig_locations[i]);	
		m_pigPointers.push_back(new_pig);
	}

	m_gameOver = false;	// haven't lost yet.

}


// destructor for the level which clears up anything I have allocated to the heap 
Level::~Level()
{
	// Got to tidy up the pigs as they have been allocated on the heap.
	for (auto pig : m_pigPointers) delete pig;

	// In future I (William) suggest you don't use raw pointers and instead use smart pointers
	// this will stop you having to deallocate memory -- std::unique_ptr will be your new BFF, in time.
}

// handle user input
void Level::handleInput(float dt)
{
	if (m_gameOver) return;
	m_sheep.handleInput(dt);
}

// Update game objects
void Level::update(float dt)
{
	if (m_gameOver) return;

	// update sheep and pigs
	m_sheep.update(dt);
	for (auto pig : m_pigPointers) pig->update(dt);

	// Compute current speed
	sf::Vector2f vel = m_sheep.getVelocity();
	float speed = std::sqrt(vel.x * vel.x + vel.y * vel.y);

	// Update the UI Text
	if (m_speedText) {
		m_speedText->text.setString("Speed: " + std::to_string(static_cast<int>(speed)));
	}

	// keep the sheep centered
	sf::Vector2f camCenter = m_sheep.getPosition();

	// camera shake
	if (m_shakeTimer > 0.f)
	{
		m_shakeTimer -= dt;

		float offsetX = (rand() % 10 - 5.f) * SHAKE_INTENSITY;
		float offsetY = (rand() % 10 - 5.f) * SHAKE_INTENSITY;

		camCenter += { offsetX, offsetY };
	}

	// BOUND CAMERA TO FARMYARD
	sf::Vector2f viewSize = m_window.getView().getSize();
	float halfWidth = viewSize.x / 2.f;
	float halfHeight = viewSize.y / 2.f;

	// Clamp X
	if (camCenter.x - halfWidth < 0.f)
		camCenter.x = halfWidth;
	else if (camCenter.x + halfWidth > m_background.getSize().x)
		camCenter.x = m_background.getSize().x - halfWidth;

	// Clamp Y
	if (camCenter.y - halfHeight < 0.f)
		camCenter.y = halfHeight;
	else if (camCenter.y + halfHeight > m_background.getSize().y)
		camCenter.y = m_background.getSize().y - halfHeight;

	// Apply view
	sf::View view = m_window.getView();
	view.setCenter(camCenter);
	m_window.setView(view);

	// handle collisions
	for (auto pig : m_pigPointers)
	{
		pig->update(dt);

		// collision check
		if (Collision::checkBoundingBox(*pig, m_sheep))
		{
			pig->collisionResponse(m_sheep);
			m_sheep.collisionResponse(*pig);

			// camera shake
			m_shakeTimer = SHAKE_TIME;
		}
	}
}

// Render level
void Level::render()
{
	beginDraw();

	// world camera
	m_window.draw(m_background);
	for (auto pig : m_pigPointers) m_window.draw(*pig);
	m_window.draw(m_sheep);

	// Switch to static UI view for the speed text
	sf::View worldView = m_window.getView();
	m_window.setView(m_window.getDefaultView()); // Reset view to draw UI in fixed position

	if (m_speedText) {
		m_window.draw(m_speedText->text);
	}

	m_window.setView(worldView); // Restore world view

	endDraw();
}

