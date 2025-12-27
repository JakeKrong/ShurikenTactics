#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

#include "World.h"
#include "StateManager.h"
#include "TextureManager.h"
#include "SoundManager.h"
#include "FontManager.h"

#include "MainMenuState.h" //Can include all states into one file later
#include "PlayingState.h"

class Game {
public:
	Game();
	~Game() = default;
	void Run();

	World& GetWorld();
	sf::RenderWindow& GetWindow();
	
	TextureManager& GetTextureManager();
	SoundManager& GetSoundManager();
	FontManager& GetFontManager();

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