#pragma once
#include "Game.h"
#include "Prefabs.h"

class LevelGenerator {
public:
	LevelGenerator(Game* game) :
		m_prefCtx( game->GetWorld(), &game->GetTextureManager(), &game->GetSoundManager(), &game->GetFontManager())
	{};

	bool LoadLevel(int level);
	void CreateLevelBorders();
	void CreateStartingDoors();
	void CreateControlsDisplay();

private:
	PrefabContext m_prefCtx;
};