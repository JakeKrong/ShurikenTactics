#include "StateManager.h"

void StateManager::Update(const float deltaTime, sf::RenderWindow& renderWindow) {
	m_CurrentState->Update(renderWindow, deltaTime);

	if (m_IsPendingStateChange) {
		ChangeState(std::move(m_PendingState));
		m_IsPendingStateChange = false;
	}
}

void StateManager::Render(sf::RenderWindow& renderWindow, const float& deltaTime) {
	m_CurrentState->Render(renderWindow, deltaTime);
}

void StateManager::HandleEvent(const sf::Event& event) {
	m_CurrentState->HandleEvents(event);
}

void StateManager::ChangeState(Scope<IState> newState) {
	if (m_CurrentState) m_CurrentState->Exit();
	m_CurrentState = std::move(newState);
	if  (m_CurrentState) m_CurrentState->Enter();
}

void StateManager::EnqueueStateChange(Scope<IState> newState) {
	m_PendingState = std::move(newState);
	m_IsPendingStateChange = true;
}