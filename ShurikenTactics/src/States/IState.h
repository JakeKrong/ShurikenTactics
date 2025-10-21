#pragma once
#include <SFML/Graphics/RenderWindow.hpp>

class Game; //Forward declaration to remove circular dependency

class IState {
public:
	virtual ~IState() = default;
	virtual void Enter() = 0;
	virtual void Exit() = 0;
	virtual void Update(sf::RenderWindow&, const float&) = 0;
	virtual void Render(sf::RenderWindow&, const float&) = 0;
	virtual void HandleEvents(const sf::Event&) = 0;
};