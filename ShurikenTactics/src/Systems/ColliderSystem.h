#include "ISystem.h"
#include "Collider.h"
#include <functional>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/CircleShape.hpp>

using CollisionKey = std::uint16_t;

struct AABB {
	sf::Vector2f pos;
	sf::Vector2f size;

	float left()   const { return pos.x; }
	float right()  const { return pos.x + size.x; }
	float top()    const { return pos.y; }
	float bottom() const { return pos.y + size.y; }
};


#pragma once
class ColliderSystem : public ISystem{
public:
	ColliderSystem();
	void Update();

private:
	bool CheckCollision(Entity, Entity);

	bool CheckRectRectCollision(const sf::FloatRect&, const sf::FloatRect&);
	bool CheckCircleCircleCollision(const sf::CircleShape&, const sf::CircleShape&);
	bool checkRectCircleCollision(const sf::FloatRect&, const sf::CircleShape&);
	std::string FindIntersectionSurface(const sf::FloatRect rect1, const sf::FloatRect rect2);
	bool AABBIntersect(const AABB&, const AABB&, AABB*);

	CollisionKey GenerateCollisionKey(ColliderType a, ColliderType b);
	void RegisterHandlers();

private:
	std::unordered_map<CollisionKey, std::function<void(Entity, Entity)>> m_CollisionHandlers;
};