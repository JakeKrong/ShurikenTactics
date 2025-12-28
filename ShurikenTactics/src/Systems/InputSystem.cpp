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
		m_S_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S);
		m_R_KeyPressed = sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R);
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

	newIntent.reset = m_R_KeyPressed; //For debugging
	newIntent.pause = m_Esc_KeyPressed;

	//Movement 
	if (!m_W_KeyPressed) {
		if (m_A_KeyPressed && !m_D_KeyPressed)
			newIntent.walkLeft = true;
		if (m_D_KeyPressed && !m_A_KeyPressed)
			newIntent.walkRight = true;
	}
	else {
		if (m_W_KeyPressed) newIntent.jump = true;
	}

	if (m_S_KeyPressed) newIntent.dropDown = true;

	// Projectile
	newIntent.isAiming = m_MouseLeftClicked;
	if (m_MouseLeftClicked) {
		m_MouseLeftHeldTime += deltaTime;
		newIntent.dropDown = false;
		if (m_MouseLeftHeldTime > 0.85f) newIntent.readyToShoot = true;
	}
		

	// projectileReleased
	if (m_MouseLeftHeld && !m_MouseLeftClicked) {
		if (m_MouseLeftHeldTime > 0.85f) newIntent.projectileReleased = true;
		m_MouseLeftHeldTime = 0.0f;
	}

	m_MouseLeftHeld = m_MouseLeftClicked;

	m_Intent = newIntent;
}


void InputSystem::HandleEvents(const sf::Event& event) { }
void InputSystem::SetKeyboardDisabled(bool boolean) { m_KeyboardDisabled = boolean; }
void InputSystem::SetMouseDisabled(bool boolean) { m_MouseDisabled = boolean; }
void InputSystem::SetWindowFocused(bool boolean) { m_WindowFocused = boolean; }