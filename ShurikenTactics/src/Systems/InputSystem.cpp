#include "InputSystem.h"
#include "World.h"
#include "Components.h"

void InputSystem::Update(sf::RenderWindow& renderWindow) {
	if (!m_World) return;

	if (!m_MouseDisabled) {
		m_MouseLeftClicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
		m_MousePosition = sf::Mouse::getPosition(renderWindow);
	}
	if (!m_KeyboardDisabled) {
		m_Esc_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
		m_A_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
		m_D_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
	}
	for (const Entity& entity : m_Entities) {
		Transform& transComp = m_World->GetComponent<Transform>(entity);
		Renderable& renderComp = m_World->GetComponent<Renderable>(entity);
		if (!m_KeyboardDisabled) {
			if (m_A_KeyPressed) {
				transComp.position.x -= 5;
				renderComp.flipX = true;
			}
			if (m_D_KeyPressed) {
				transComp.position.x += 5;
				renderComp.flipX = false;
			}
		}
	}
}

void InputSystem::HandleEvents(const sf::Event& event) { }

void InputSystem::SetKeyboardDisabled(bool boolean) { m_KeyboardDisabled = boolean; }