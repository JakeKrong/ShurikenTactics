#pragma once
#include <SFML/System/Vector2.hpp>

struct Transform {
	sf::Vector2f position{ 0.0f, 0.0f };
	sf::Vector2f scale{ 1.0f, 1.0f };
	float rotation{ 0.0f };
};