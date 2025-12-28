#pragma once
#include "Types.h"
#include "IState.h"

#include "RenderSystem.h"
#include "InputSystem.h"
#include "ColliderSystem.h"
#include "PhysicsSystem.h"
#include "LifetimeSystem.h"
#include "EnemySystem.h"
#include "UISystem.h"

class PlayingState : public IState {
public:
	PlayingState(Game* game, int level = 1);
	void Enter() override;
	void Exit() override;
	void Update(sf::RenderWindow&, const float& deltaTime) override;
	void Render(sf::RenderWindow&, float) override;
	void HandleEvents(const sf::Event& event) override;

	// Gameplay Functions
	void UpdatePlayerState(sf::RenderWindow&);
	void ThrowShuriken(sf::RenderWindow&, sf::Vector2f);
	void SpawnTarget();

	void SetInputDisabled(bool);
	void PauseGame();
	void EndGame(bool);
	void EnqueueGameOver();
	void NextStage();

private:
	Game* m_Game;
	
	Ref<RenderSystem> m_RenderSystem;
	Ref<InputSystem> m_InputSystem;
	Ref<ColliderSystem> m_ColliderSystem;
	Ref<PhysicsSystem> m_PhysicsSystem;
	Ref<LifetimeSystem> m_LifetimeSystem;
	Ref<EnemySystem> m_EnemySystem;

	Ref<UISystem> m_UISystem;

	float shurikenCD{ 0.0f }; //Consider change

	bool m_GamePaused{ false };
	bool m_SwitchingStage{ false };
	float m_GameOverCnt{ -1 };
	float m_NextStageCnt{ -1 };

	int m_CurrGameLevel{1};
	int m_TotalStages = 4;
};