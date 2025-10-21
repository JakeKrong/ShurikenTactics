#pragma once
#include "Types.h"
#include "IState.h"

#include "RenderSystem.h"
#include "InputSystem.h"
#include "ColliderSystem.h"
#include "PhysicsSystem.h"
#include "LifetimeSystem.h"

class PlayingState : public IState {
public:
	PlayingState(Game* game);
	void Enter() override;
	void Exit() override;
	void Update(sf::RenderWindow&, const float& deltaTime) override;
	void Render(sf::RenderWindow&, const float&) override;
	void HandleEvents(const sf::Event& event) override;

	// Gameplay Functions
	void UpdatePlayerState();
	void ThrowShuriken(sf::Vector2f);
	void SpawnTarget();

private:
	Game* m_Game;
	
	Ref<RenderSystem> m_RenderSystem;
	Ref<InputSystem> m_InputSystem;
	Ref<ColliderSystem> m_ColliderSystem;
	Ref<PhysicsSystem> m_PhysicsSystem;
	Ref<LifetimeSystem> m_LifetimeSystem;

	float shurikenCD{0.0f};
};