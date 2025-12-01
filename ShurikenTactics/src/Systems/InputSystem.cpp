#include "InputSystem.h"
#include "World.h"
#include "Components.h"

#include <iostream>

void InputSystem::Update(sf::RenderWindow& renderWindow, float deltaTime) {
	if (!m_World || !m_WindowFocused) {
		m_Intent = {};
		return;
	}

	//  *** Raw input polling *** //
	if (!m_KeyboardDisabled) {
		m_Esc_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape);
		m_A_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A);
		m_D_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D);
		m_W_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W);
	}
	else {
		m_A_KeyPressed = m_D_KeyPressed = m_W_KeyPressed = false;
	}

	if (!m_MouseDisabled) {
		m_MouseLeftClicked = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
		m_MousePosition = sf::Mouse::getPosition(renderWindow);
	}
	else m_MouseLeftClicked = false;

	// *** Input intent resolution *** //
	PlayerInputIntent newIntent;
	newIntent.mousePos = m_MousePosition;

	//Movement 
	if (!m_W_KeyPressed) {
		if (m_A_KeyPressed && !m_D_KeyPressed)
			newIntent.walkLeft = true;
		if (m_D_KeyPressed && !m_A_KeyPressed)
			newIntent.walkRight = true;
	}
	else {
		newIntent.jump = m_W_KeyPressed;
	}

	// Projectile
	newIntent.isAiming = m_MouseLeftClicked;
	if (m_MouseLeftClicked) m_MouseLeftHeldTime += deltaTime;

	// projectileReleased
	if (m_MouseLeftHeld && !m_MouseLeftClicked) {
		if (m_MouseLeftHeldTime >= 0.2f) newIntent.projectileReleased = true;
		m_MouseLeftHeldTime = 0.0f;
	}

	m_MouseLeftHeld = m_MouseLeftClicked;

	m_Intent = newIntent;

	// *** Process input intents logic *** //

	for (const Entity& entity : m_Entities) {
		Transform& transComp = m_World->GetComponent<Transform>(entity);
		Physics& physicsComp = m_World->GetComponent<Physics>(entity);
		Renderable& renderComp = m_World->GetComponent<Renderable>(entity);

		if ((!m_Intent.walkLeft && !m_Intent.walkRight) || m_Intent.isAiming) {
			physicsComp.velocity.x = 0;
		}

		if (!m_KeyboardDisabled && !m_Intent.isAiming) {
			if (m_Intent.walkLeft) {
				physicsComp.velocity.x = -300;
				renderComp.flipX = true;
			}
			if (m_Intent.walkRight) {
				physicsComp.velocity.x = 300;
				renderComp.flipX = false;
			}
		}
	}
}


void InputSystem::HandleEvents(const sf::Event& event) { }
void InputSystem::SetKeyboardDisabled(bool boolean) { m_KeyboardDisabled = boolean; }
void InputSystem::SetWindowFocused(bool boolean) { m_WindowFocused = boolean; }