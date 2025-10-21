#pragma once
#include <vector>
#include <variant>
#include <typeindex>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/CircleShape.hpp>

using ColliderShape = std::variant<sf::FloatRect, sf::CircleShape>;

enum class ColliderType : std::uint16_t {
	PlayerBox,
	ObstacleBox,
	ProjectileBox,
	TargetBox
};

struct Collider {
	std::vector<ColliderShape> entityColliders;
	ColliderType type{};

	void AddRect(sf::FloatRect collider) {
		entityColliders.emplace_back(collider);
	}

	void AddCircle(sf::CircleShape collider) {
		entityColliders.emplace_back(collider);
	}
};