#pragma once
#include "IState.h"
#include "RenderSystem.h"
#include "UISystem.h"

class MainMenuState : public IState {
public:

	MainMenuState(Game* game);
	void Enter() override;
	void Exit() override;
	void Update(sf::RenderWindow&, const float&) override;
	void Render(sf::RenderWindow&, const float&) override;
	void HandleEvents(const sf::Event& event) override;

private:
	Game* m_Game;

	Ref<RenderSystem> m_RenderSystem;
	Ref<UISystem> m_UISystem;
};