#pragma once
#include <SFML/Window/Event.hpp>
#include "ISystem.h"

class UISystem : public ISystem{
public:
	void HandleEvents(const sf::Event& event, const sf::Vector2f& mousePos);
private:

};