#pragma once
#include <SFML/System/Vector2.hpp>

struct Physics {
	sf::Vector2f velocity{ 0.0f, 0.0f };
	sf::Vector2f acceleration{ 0.0f, 0.0f };
	float mass{ 1.0f };
	bool affectedByGravity{ false };
	bool isGrounded{ false };
};