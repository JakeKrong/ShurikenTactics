#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "ISystem.h"

struct PlayerInputIntent {
	bool walkLeft = false;
	bool walkRight = false;
	bool jump = false;
	bool dropDown = false;

	bool isAiming = false;
	bool projectileReleased = false;

	sf::Vector2i mousePos;
};

class InputSystem :public ISystem {
public:
	void Update(sf::RenderWindow&, float);
	void HandleEvents(const sf::Event& event);

	void SetKeyboardDisabled(bool);
	void SetWindowFocused(bool);

	const PlayerInputIntent& GetIntent() const { return m_Intent; }

private:
	PlayerInputIntent m_Intent{};

	bool m_Esc_KeyPressed{ false };
	bool m_A_KeyPressed{ false };
	bool m_D_KeyPressed{ false };
	bool m_W_KeyPressed{ false };
	bool m_S_KeyPressed{ false };

	bool m_MouseLeftClicked{ false };
	bool m_MouseLeftHeld{ false };
	float m_MouseLeftHeldTime = 0;

	sf::Vector2i m_MousePosition{ 0, 0 };

	bool m_MouseDisabled{ false };
	bool m_KeyboardDisabled{ false };
	bool m_WindowFocused{ true };
	//bool m_UiMode{ false };
};