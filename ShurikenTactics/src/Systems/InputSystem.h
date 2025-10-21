#pragma once
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Window/Mouse.hpp>

#include "ISystem.h"

class InputSystem :public ISystem {
public:
	void Update(sf::RenderWindow&);
	void HandleEvents(const sf::Event& event);

	bool WasLeftClicked() const { return m_MouseLeftClicked; }
	sf::Vector2i GetMousePosition() const { return m_MousePosition; }
	void SetKeyboardDisabled(bool);

	bool m_A_KeyPressed{ false };
	bool m_D_KeyPressed{ false };

private:
	bool m_Esc_KeyPressed{ false };
	//bool m_A_KeyPressed{ false };
	//bool m_D_KeyPressed{ false };
	bool m_MouseLeftClicked{ false };
	sf::Vector2i m_MousePosition{0, 0};

	bool m_MouseDisabled{ false };
	bool m_KeyboardDisabled{ false };
	bool m_UiMode{ false };
};