#pragma once
#include <vector>
#include <variant>
#include <typeindex>
#include <functional>
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
	std::function<void(Entity ent)> OnCollision = nullptr;
	std::vector<std::pair<ColliderShape, sf::Vector2f>> entityColliders;
	
	int frameBuffer = 0;
	ColliderType type{};

	bool canPhaseThroughPlatform = false; //Set to false when standing on solid floors
	bool phaseThroughPlatform = false;
	bool isDanger = false; //To indicate danger towards player (if true, does not hit enemy)

	void AddRect(std::pair<sf::FloatRect, sf::Vector2f>collider) {
		entityColliders.emplace_back(collider);
	}

	void AddCircle(std::pair<sf::CircleShape, sf::Vector2f>collider) {
		entityColliders.emplace_back(collider);
	}
};