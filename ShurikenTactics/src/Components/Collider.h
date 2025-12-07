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
	TargetBox,
	EnemyBox,
	PlatformBox
};

struct Collider {
	std::vector<std::pair<ColliderShape, sf::Vector2f>> entityColliders;
	ColliderType type{};

	bool canPhaseThroughPlatform = false; //Set to false when standing on solid floors
	bool phaseThroughPlatform = false;
	int frameBuffer = 0;
	bool isDanger = false; //To indicate danger towards player

	void AddRect(std::pair<sf::FloatRect, sf::Vector2f>collider) {
		entityColliders.emplace_back(collider);
	}

	void AddCircle(std::pair<sf::CircleShape, sf::Vector2f>collider) {
		entityColliders.emplace_back(collider);
	}
};