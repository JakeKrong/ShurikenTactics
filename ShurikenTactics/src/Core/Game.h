#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "World.h"
#include "StateManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "FontManager.h"
#include "MainMenuState.h" //Can include all states into one file later

class Game {
public:
	Game();
	~Game() = default;
	void Run();

	sf::RenderWindow& GetWindow();
	World& GetWorld();
	

private:
	sf::RenderWindow m_Window;
	World m_World;
	StateManager m_StateManager;
	TextureManager m_TextureManager;
	SoundManager m_SoundManager;
	FontManager m_FontManager;

	friend class MainMenuState;
	friend class PlayingState;
};