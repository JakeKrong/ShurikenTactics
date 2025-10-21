#pragma once
#include "Types.h"
#include "IState.h"


class StateManager {
public:
	void Update(float deltaTime, sf::RenderWindow&);
	void Render(sf::RenderWindow&, const float&);
	void HandleEvent(const sf::Event&);
	void ChangeState(Scope<IState> newState);
	void EnqueueStateChange(Scope<IState> newState);

private:
	Scope<IState> m_CurrentState;
	bool m_IsPendingStateChange{ false };
	Scope<IState> m_PendingState;
};