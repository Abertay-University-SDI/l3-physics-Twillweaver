#include "Level.h"

Level::Level(sf::RenderWindow& hwnd, Input& in) :
	BaseLevel(hwnd, in)
{
	// setup background
	float background_size = 1024;
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

	// keep the sheep centered
	sf::Vector2f pos = m_sheep.getPosition();
	sf::View view = m_window.getView();
	view.setCenter(pos);
	m_window.setView(view);

	m_sheep.update(dt);
	
	for (auto pig : m_pigPointers)
	{
		pig->update(dt);

		// ---- COLLISION CHECK ----
		if (Collision::checkBoundingBox(*pig, m_sheep))
		{
			pig->collisionResponse(m_sheep);
		}
	}
}

// Render level
void Level::render()
{
	beginDraw();
	m_window.draw(m_background);
	for (auto pig : m_pigPointers) m_window.draw(*pig);
	m_window.draw(m_sheep);
	endDraw();
}

