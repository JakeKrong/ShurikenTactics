#pragma once
#include "Game.h"

class LevelGenerator {
public:
	LevelGenerator(Game* game) :
		m_Game(game)
	{};

	bool LoadLevel(int level);
	void CreateLevelBorders();
	void CreateStartingDoors();
	void CreateControlsDisplay();

private:
	Game* m_Game;
};