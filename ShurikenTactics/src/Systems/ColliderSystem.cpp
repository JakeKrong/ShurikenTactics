#include "ColliderSystem.h"
#include "World.h"

#include "Physics.h"
#include "Transform.h"
#include "Renderable.h"
#include "Lifetime.h"

#include <iostream>
#include <algorithm>

#include <string>

ColliderSystem::ColliderSystem() {
	RegisterHandlers();
}

void ColliderSystem::Update() {
	if (!m_World) return;

	for (Entity entity : m_Entities) {
		auto& transform = m_World->GetComponent<Transform>(entity);
		auto& collider = m_World->GetComponent<Collider>(entity);
		for (auto& shapeVariant : collider.entityColliders) {
			std::visit([&](auto& shape) {
				using ShapeType = std::decay_t<decltype(shape)>;

				if constexpr (std::is_same_v<ShapeType, sf::FloatRect>) {
					shape = sf::FloatRect({ transform.position.x,
						transform.position.y },
						{ shape.size.x, shape.size.y });
				}
				else if constexpr (std::is_same_v<ShapeType, sf::CircleShape>) {
					shape.setPosition(transform.position);				}
				}, shapeVariant);
		}
	}

	//Initialize vector of m_Entities set to iterate over (check if can declare m_Entities as vector later)
	std::vector<Entity> entityList(m_Entities.begin(), m_Entities.end());

	for (int i = 0; i < entityList.size(); i++) {
		for (int j = i + 1; j < entityList.size(); j++) {
			const Entity& entA = entityList[i];
			const Entity& entB = entityList[j];

			try {
				if (CheckCollision(entA, entB)) {
					CollisionKey collPairing = GenerateCollisionKey(m_World->GetComponent<Collider>(entA).type,
																	m_World->GetComponent<Collider>(entB).type);
					auto handlerIt = m_CollisionHandlers.find(collPairing);
					handlerIt->second(entA, entB);
					//std::cout << "Collision";
				}
			}
			catch (const std::exception& e) {
				std::cerr << "[Collision System] Exception during collision checking/handling:" << e.what() << "\n";
			}
		}
	}
}

bool ColliderSystem::CheckCollision(Entity entA, Entity entB) {
	Collider& colliderA = m_World->GetComponent<Collider>(entA);
	Collider& colliderB = m_World->GetComponent<Collider>(entB);

	//If collision pairing not found, return false
	if (m_CollisionHandlers.find(GenerateCollisionKey(colliderA.type, colliderB.type)) == m_CollisionHandlers.end()) return false;
	for (const ColliderShape& colA : colliderA.entityColliders) {
		for (const ColliderShape& colB : colliderB.entityColliders) {
			bool collided = std::visit([&](auto&& a, auto&& b)->bool {
				using A = std::decay_t<decltype(a)>;
				using B = std::decay_t<decltype(b)>;
				if constexpr (std::is_same_v<A, sf::FloatRect> && std::is_same_v<B, sf::FloatRect>)
					return CheckRectRectCollision(a, b);
				else if constexpr (std::is_same_v<A, sf::CircleShape> && std::is_same_v<B, sf::CircleShape>)
					return CheckCircleCircleCollision(a, b);
				else {
					if constexpr (std::is_same_v<A, sf::CircleShape>) return checkRectCircleCollision(b, a);
					else return checkRectCircleCollision(a, b);
				}
			}, colA, colB);

			if (collided) {
				return true;
			}
		}
	}
	return false;
}

bool ColliderSystem::CheckRectRectCollision(const sf::FloatRect& rectA, const sf::FloatRect& rectB) {
	auto intersect = rectA.findIntersection(rectB);
	return intersect.has_value();
}


bool ColliderSystem::CheckCircleCircleCollision(const sf::CircleShape& circleA, const sf::CircleShape& circleB) {
	sf::Vector2f centerA = circleA.getPosition() + sf::Vector2f(circleA.getRadius(), circleA.getRadius());
	sf::Vector2f centerB = circleB.getPosition() + sf::Vector2f(circleB.getRadius(), circleB.getRadius());

	float dx = centerA.x - centerB.x;
	float dy = centerA.y - centerB.y;
	float distSq = dx * dx + dy * dy;
	float radiusSum = circleA.getRadius() + circleB.getRadius();
	return distSq <= radiusSum * radiusSum;
}


bool ColliderSystem::checkRectCircleCollision(const sf::FloatRect& rect, const sf::CircleShape& circle) {
	sf::Vector2f circleCenter = circle.getPosition() + sf::Vector2f(circle.getRadius(), circle.getRadius());

	float closestX = std::clamp(circleCenter.x, rect.position.x, rect.position.x + rect.size.x);
	float closestY = std::clamp(circleCenter.y, rect.position.y, rect.position.y + rect.size.y);

	float dx = circleCenter.x - closestX;
	float dy = circleCenter.y - closestY;
	return (dx * dx + dy * dy) <= (circle.getRadius() * circle.getRadius());
}

CollisionKey ColliderSystem::GenerateCollisionKey(ColliderType a, ColliderType b) {
	return (static_cast<CollisionKey>(a) << 8 | static_cast<CollisionKey>(b));
}

//Set up collision logic for relevant collider type pairings
void ColliderSystem::RegisterHandlers() {
	//Obstacle -> Projectile Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::ObstacleBox, ColliderType::ProjectileBox)] = [this](Entity a, Entity b)
		{
			sf::FloatRect obstacleCollider{ m_World->GetComponent<Transform>(a).position , m_World->GetComponent<Renderable>(a).size };
			sf::FloatRect projectileCollider{ m_World->GetComponent<Transform>(b).position, {30,30} };

			AABB obstacleAABB{ obstacleCollider.position, obstacleCollider.size };
			AABB projectileAABB{ projectileCollider.position, projectileCollider.size };
			AABB overlap;
			if (!AABBIntersect(obstacleAABB , projectileAABB, & overlap)) return;

			sf::Vector2f& proVelocity = m_World->GetComponent<Physics>(b).velocity;
			if (overlap.size.x < overlap.size.y) proVelocity = { -proVelocity.x, proVelocity.y };
			else proVelocity = { proVelocity.x, -proVelocity.y };

			m_World->GetComponent<Lifetime>(b).durability -= 1;
		};

	//Player -> Obstacle Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::ObstacleBox, ColliderType::PlayerBox)] = [this](Entity obs, Entity plyr)
		{
			auto& obstacleTrans = m_World->GetComponent<Transform>(obs);
			auto& playerTrans = m_World->GetComponent<Transform>(plyr);

			auto& obstacleRenderable = m_World->GetComponent<Renderable>(obs);
			auto& playerRenderable = m_World->GetComponent<Renderable>(plyr);

			AABB obstacleAABB{ obstacleTrans.position, obstacleRenderable.size };
			AABB playerAABB{ playerTrans.position,  playerRenderable.size };

			AABB overlap;
			if (!AABBIntersect(obstacleAABB, playerAABB, &overlap)) return;

			//std::cout << "\nOverlap Size: " << overlap.size.x << "," << overlap.size.y << " | " << "Overlap Position: " << overlap.pos.x << "," << overlap.pos.y;

			// choose smaller penetration axis
			if (overlap.size.x < overlap.size.y) {
				// horizontal collision
				if (playerAABB.left() < obstacleAABB.left()) {
					// player is left of obstacle -> push left
					playerTrans.position.x -= overlap.size.x;
				}
				else {
					// player is right of obstacle -> push right
					playerTrans.position.x += overlap.size.x;
				}
				// zero horizontal velocity
				m_World->GetComponent<Physics>(plyr).velocity.x = 0.f;
			}
			else {
				// vertical collision (push out on y)
				if (playerAABB.top() < obstacleAABB.top()) {
					// player is above obstacle -> land on top
					playerTrans.position.y -= overlap.size.y;
					// set grounded flag (optional)
					m_World->GetComponent<Physics>(plyr).isGrounded = true;
				}
				else {
					// player hit obstacle from below
					playerTrans.position.y += overlap.size.y;
				}
				// zero vertical velocity
				m_World->GetComponent<Physics>(plyr).velocity.y = 0.f;
			}
		};

	//Target -> Obstacle Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::ObstacleBox, ColliderType::TargetBox)] = [this](Entity obs, Entity target)
		{
			auto& obstacleTrans = m_World->GetComponent<Transform>(obs);
			auto& targetTrans = m_World->GetComponent<Transform>(target);

			auto& obstacleRenderable = m_World->GetComponent<Renderable>(obs);
			auto& targetRenderable = m_World->GetComponent<Renderable>(target);

			AABB obstacleAABB{ obstacleTrans.position, obstacleRenderable.size };
			AABB targetAABB{ targetTrans.position,  targetRenderable.size };

			AABB overlap;
			if (!AABBIntersect(obstacleAABB, targetAABB, &overlap)) return;
			if (targetAABB.top() < obstacleAABB.top()) {
				targetTrans.position.y -= overlap.size.y;
				m_World->GetComponent<Physics>(target).isGrounded = true;
			}
			else {
				// player hit obstacle from below
				targetTrans.position.y += overlap.size.y;
			}
			// zero vertical velocity
			m_World->GetComponent<Physics>(target).velocity.y = 0.f;
		};

	m_CollisionHandlers[GenerateCollisionKey(ColliderType::TargetBox, ColliderType::ProjectileBox)] = [this](Entity a, Entity b)
		{
			m_World->GetComponent<Lifetime>(a).durability -= 3;
			m_World->GetComponent<Lifetime>(b).durability -= 3;
		};
}

bool ColliderSystem::AABBIntersect(const AABB& a, const AABB& b, AABB* outOverlap = nullptr) {
	float overlapX = std::min(a.right(), b.right()) - std::max(a.left(), b.left());
	float overlapY = std::min(a.bottom(), b.bottom()) - std::max(a.top(), b.top());

	if (overlapX > 0.0f && overlapY > 0.0f) {
		if (outOverlap) {
			// overlap pos = max(lefts, tops), size = overlaps
			outOverlap->pos.x = std::max(a.left(), b.left());
			outOverlap->pos.y = std::max(a.top(), b.top());
			outOverlap->size.x = overlapX;
			outOverlap->size.y = overlapY;
		}
		return true;
	}
	return false;
}