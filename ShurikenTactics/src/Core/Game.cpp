#include "Game.h"

Game::Game() :
	m_Window(sf::RenderWindow(sf::VideoMode({1280,720}), "Shuriken Tactics", sf::Style::Default))
{
	m_Window.setFramerateLimit(60);
}

void Game::Run() { 
	sf::Clock clock;

	//Start in main menu
	m_StateManager.ChangeState(CreateScope<MainMenuState>(this));

	//Run main loop
	while (m_Window.isOpen()) {
		float deltaTime = clock.restart().asSeconds(); //Get time between last loop
		if (deltaTime > 0.1f) deltaTime = 0.1f; //Clamp deltaTime to max of 0.1 seconds
		while (auto optEvent = m_Window.pollEvent()) { //Poll event until none left
			if (optEvent->is<sf::Event::Closed>()) {
				m_Window.close();
			}
			m_StateManager.HandleEvent(*optEvent);
		}
		m_StateManager.Update(deltaTime, m_Window);
		m_StateManager.Render(m_Window, deltaTime);
	}
}

sf::RenderWindow& Game::GetWindow() { return m_Window; };

World& Game::GetWorld() { return m_World; };