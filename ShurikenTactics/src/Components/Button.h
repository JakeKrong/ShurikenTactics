#pragma once

#include "SFML/System/Vector2.hpp"
#include <functional>

struct Button {
	sf::Vector2f size{ 10, 10 }; //Maybe can decouple and directly use renderable's size
	std::function<void()> onButtonClick;
	bool isEnabled{ true };
};