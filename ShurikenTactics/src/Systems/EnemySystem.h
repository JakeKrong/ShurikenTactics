#pragma once
#include "ISystem.h"
#include "Enemy.h"

class TextureManager; //Forward declaration
class SoundManager;

class EnemySystem : public ISystem {
public:
	void Update(const float& deltaTime);
	void UpdateIdle(Entity enemyEnt);
	void UpdateAlert(Entity enemyEnt);
	void UpdateChase(Entity enemyEnt);
	void UpdateGuard(Entity enemyEnt);
	void UpdateAttack(Entity enemyEnt);
	void UpdateDraw(Entity enemyEnt);
	void UpdateDead(Entity enemyEnt);

	bool PlayerInLOS(Entity enemyEnt);
	bool HasWalkablePath (Entity enemyEnt);

	void PreviewArrow(Entity enemyEnt);
	void FireArrow(Entity enemyEnt);
	void TurnEnemy(Entity enemyEnt, bool syncDir = false);
	void SetTextureManager(TextureManager* tm);
	void SetSoundManager(SoundManager* sm);
	
private:
	TextureManager* m_TextureManager = nullptr;
	SoundManager* m_SoundManager = nullptr;
};

