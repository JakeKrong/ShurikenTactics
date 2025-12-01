#pragma once
#include "ISystem.h"
#include "Collider.h"
#include <functional>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/CircleShape.hpp>

using CollisionKey = std::uint16_t;

class ColliderSystem : public ISystem{
public:
	ColliderSystem();
	void Update();

private:
	bool CheckCollision(Entity, Entity);

	bool CheckRectRectCollision(const sf::FloatRect&, const sf::FloatRect&);
	bool CheckCircleCircleCollision(const sf::CircleShape&, const sf::CircleShape&);
	bool checkRectCircleCollision(const sf::FloatRect&, const sf::CircleShape&);
	bool AABBIntersect(const AABB&, const AABB&, AABB*);

	CollisionKey GenerateCollisionKey(ColliderType a, ColliderType b);
	void RegisterHandlers();

private:
	std::unordered_map<CollisionKey, std::function<void(Entity, Entity)>> m_CollisionHandlers;
};