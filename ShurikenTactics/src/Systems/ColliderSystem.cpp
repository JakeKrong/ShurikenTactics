#include "ColliderSystem.h"
#include "World.h"
#include "Types.h"

#include "Components.h"

#include <iostream>
#include <algorithm>

#define PI 3.14159265f

ColliderSystem::ColliderSystem() {
	RegisterHandlers();
}

void ColliderSystem::Update() {
	if (!m_World) return;

	//Update collider position to match with transform
	for (Entity entity : m_Entities) {

		auto& transform = m_World->GetComponent<Transform>(entity);
		auto& collider = m_World->GetComponent<Collider>(entity);

		if (collider.frameBuffer > 0) collider.frameBuffer -= 1; //Decrement collision detection buffer

		collider.canPhaseThroughPlatform = true;

		for (auto& [shapeVariant, offset] : collider.entityColliders) {
			std::visit([&](auto& shape) {
				using ShapeType = std::decay_t<decltype(shape)>;

				if constexpr (std::is_same_v<ShapeType, sf::FloatRect>) {
					shape = sf::FloatRect({ transform.position.x + offset.x,
						transform.position.y + offset.y },
						{ shape.size.x, shape.size.y });
				}
				else if constexpr (std::is_same_v<ShapeType, sf::CircleShape>) {
					shape.setPosition({ transform.position.x + offset.x , transform.position.y + offset.y }); }
				}, shapeVariant);
		}
	}

	for (int i = 0; i < m_Entities.size(); i++) {
		for (int j = i + 1; j < m_Entities.size(); j++) {
			const Entity& entA = m_Entities[i];
			const Entity& entB = m_Entities[j];

			try {
				if (CheckCollision(entA, entB)) {
					CollisionKey collisionPairing = GenerateCollisionKey(m_World->GetComponent<Collider>(entA).type,
																	m_World->GetComponent<Collider>(entB).type);

					auto collisionHandler = m_CollisionHandlers.find(collisionPairing)->second;

					if (m_World->GetComponent<Collider>(entA).type < m_World->GetComponent<Collider>(entB).type)
						collisionHandler(entA, entB);
					else
						collisionHandler(entB, entA);
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

	if (colliderA.frameBuffer > 0 || colliderB.frameBuffer > 0) return false; //If have collision buffer, skip

	//If collision pairing not found, return false
	if (m_CollisionHandlers.find(GenerateCollisionKey(colliderA.type, colliderB.type)) == m_CollisionHandlers.end()) return false;
	for (const auto& [colA, _] : colliderA.entityColliders) {
		for (const auto& [colB, _] : colliderB.entityColliders) {
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
	if (a > b) std::swap(a, b);
	return (static_cast<CollisionKey>(a) << 8 | static_cast<CollisionKey>(b));
}

//Set up collision logic for relevant collider type pairings
void ColliderSystem::RegisterHandlers() {
	// === Player === //
	//Player -> Obstacle Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::PlayerBox, ColliderType::ObstacleBox)] = [this](Entity plyr, Entity obs)
		{
			auto& obstacleTrans = m_World->GetComponent<Transform>(obs);
			auto& playerTrans = m_World->GetComponent<Transform>(plyr);

			auto& obstacleRenderable = m_World->GetComponent<Renderable>(obs);
			auto& playerRenderable = m_World->GetComponent<Renderable>(plyr);

			auto& playerPhysics = m_World->GetComponent<Physics>(plyr);

			AABB obstacleAABB{ obstacleTrans.position, obstacleRenderable.size };
			AABB playerAABB{ playerTrans.position,  playerRenderable.size };

			AABB overlap;
			if (!AABBIntersect(obstacleAABB, playerAABB, &overlap)) return;

			// Determine the axis to resolve on.
			bool resolveX = overlap.size.x < overlap.size.y;

			// Edge case fix: When moving up (jumping) and detect a y-collision, change to horizontal instead
			if (!resolveX) {
				bool isPlayerAbove = playerAABB.top() < obstacleAABB.top();
				bool isJumping = playerPhysics.velocity.y < 0.f;

				if (isPlayerAbove && isJumping) {
					resolveX = true; // Force horizontal slide
				}
			}

			if (resolveX) {
				// --- Horizontal Collision ---
				if (playerAABB.left() < obstacleAABB.left()) {
					playerTrans.position.x -= overlap.size.x; // Push Left
				}
				else {
					playerTrans.position.x += overlap.size.x; // Push Right
				}
				playerPhysics.velocity.x = 0.f;
			}
			else {
				// --- Vertical Collision ---
				if (playerAABB.top() < obstacleAABB.top()) {
					// Player hit obstacle from above
					playerTrans.position.y -= overlap.size.y - .1f;
					playerPhysics.isGrounded = true;
					playerPhysics.velocity.y = 0.f;
					m_World->GetComponent<Collider>(plyr).canPhaseThroughPlatform = false;
				}
				else {
					// Player hit obstacle from below
					playerTrans.position.y += overlap.size.y;

					// Only stop velocity if we were actually moving up
					if (playerPhysics.velocity.y < 0.f) {
						playerPhysics.velocity.y = 0.f;
					}
				}
			}
		};

	//Player -> Platform Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::PlayerBox, ColliderType::PlatformBox)] = [this](Entity plyr, Entity plat)
		{
			auto& obstacleTrans = m_World->GetComponent<Transform>(plat);
			auto& playerTrans = m_World->GetComponent<Transform>(plyr);

			auto& obstacleRenderable = m_World->GetComponent<Renderable>(plat);
			auto& playerRenderable = m_World->GetComponent<Renderable>(plyr);

			auto& playerPhysics = m_World->GetComponent<Physics>(plyr);
			auto& playerCollider = m_World->GetComponent<Collider>(plyr);

			AABB obstacleAABB{ obstacleTrans.position, obstacleRenderable.size };
			AABB playerAABB{ playerTrans.position,  playerRenderable.size };

			AABB overlap;
			if (!AABBIntersect(obstacleAABB, playerAABB, &overlap)) return;

			// --- Vertical Collision ---
			if (playerAABB.top() < obstacleAABB.top() && std::abs(obstacleAABB.top() - playerAABB.bottom()) <= 5 
				&& playerPhysics.velocity.y >= 0 && !playerCollider.phaseThroughPlatform) {
				// Player hit obstacle from above
				playerTrans.position.y -= overlap.size.y - .1f;
				playerPhysics.isGrounded = true;
				playerPhysics.velocity.y = 0.f;
			}
		};

	//Player -> Platform Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::PlayerBox, ColliderType::ProjectileBox)] = [this](Entity a, Entity b)
		{
			if (m_World->GetComponent<Collider>(b).isDanger) {
				m_World->GetComponent<Player>(a).health = 0;
				m_World->GetComponent<Lifetime>(b).durability = 0;
			}
		};

	//Player -> Enemy Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::PlayerBox, ColliderType::EnemyBox)] = [this](Entity a, Entity b)
		{
			Enemy& enemyComp = m_World->GetComponent<Enemy>(b);
			enemyComp.inPlayerKillRange = true;
			if (enemyComp.isLethal) {
				m_World->GetComponent<Player>(a).health = 0;
			}
		};

	// === Obstacle === //
	//Obstacle -> Projectile Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::ObstacleBox, ColliderType::ProjectileBox)] = [this](Entity a, Entity b)
		{
			sf::FloatRect obstacleCollider{ m_World->GetComponent<Transform>(a).position , m_World->GetComponent<Renderable>(a).size };
			//sf::FloatRect projectileCollider{ m_World->GetComponent<Transform>(b).position, m_World->GetComponent<Renderable>(b).size };
			AABB obstacleAABB{ obstacleCollider.position, obstacleCollider.size };

			Transform projectileTrans = m_World->GetComponent<Transform>(b);
			sf::CircleShape projectileCollider = std::get<sf::CircleShape>(m_World->GetComponent<Collider>(b).entityColliders[0].first);

			sf::Vector2f circleCenter = projectileCollider.getPosition() + sf::Vector2f(projectileCollider.getRadius(), projectileCollider.getRadius());
			float closestX = std::clamp(circleCenter.x, obstacleCollider.position.x, obstacleCollider.position.x + obstacleCollider.size.x);
			float closestY = std::clamp(circleCenter.y, obstacleCollider.position.y, obstacleCollider.position.y + obstacleCollider.size.y);

			sf::Vector2f& proVelocity = m_World->GetComponent<Physics>(b).velocity;

			float distLeft = std::abs(closestX - obstacleAABB.left());
			float distRight = std::abs(closestX - obstacleAABB.right());
			float distTop = std::abs(closestY - obstacleAABB.top());
			float distBottom = std::abs(closestY - obstacleAABB.bottom());

			//std::cout << closestX << " | " << closestY << std::endl;

			//Check edge case of corners hit
			if (std::abs(std::min(distLeft, distRight) - std::min(distTop, distBottom)) < 0.2f) {	//0.2f: Arbitrary number cap to determine corner hit (difference of top/bottom & right/left obstacle intersection, can be adjusted)

				float proSpeed = proVelocity.length();
				int deflectionDegree;

				if (closestY == obstacleAABB.bottom()) {
					if (closestX == obstacleAABB.right()) deflectionDegree = 45;
					else deflectionDegree = 135;
				}
				else {
					if (closestX == obstacleAABB.left()) deflectionDegree = 235;
					else deflectionDegree = 315;
				}

				proVelocity = sf::Vector2f(proSpeed, sf::Angle(sf::degrees(deflectionDegree)));
				//float radian = deflectionDegree * PI / 180.0f;
				//proVelocity = sf::Vector2f{ std::cos(radian), std::sin(radian) } *proSpeed;


				//std::cout << "Corner Collision. " << std::min(distLeft, distRight) << " | " << std::min(distTop, distBottom) << " . " << closestX << "|" << closestY << " . " << proVelocity.x << "|" << proVelocity.y << std::endl;
				m_World->GetComponent<Lifetime>(b).durability -= 1;
				m_World->GetComponent<Collider>(b).frameBuffer = 2;
			}
			//Simple X Collision
			else if (std::min(distLeft, distRight) < std::min(distTop, distBottom) &&
				((distLeft < distRight && proVelocity.x > 0) ||
					(distRight < distLeft && proVelocity.x < 0)))
			{
				proVelocity.x = -proVelocity.x;
				//std::cout << "X Collision. " << std::min(distLeft, distRight) << " | " << std::min(distTop, distBottom) << " . " << closestX << "|" << closestY << " . " << proVelocity.x << "|" << proVelocity.y << std::endl;
				m_World->GetComponent<Lifetime>(b).durability -= 1;
				m_World->GetComponent<Collider>(b).frameBuffer = 2;
			}
			// Simple Y collision
			else if (std::min(distLeft, distRight) > std::min(distTop, distBottom) &&
				((distTop < distBottom && proVelocity.y > 0) ||
					(distBottom < distTop && proVelocity.y < 0)))
			{
				proVelocity.y = -proVelocity.y;
				//std::cout << "Y Collision. " << std::min(distLeft, distRight) << " | " << std::min(distTop, distBottom) << " . " << closestX << "|" << closestY << " . " << proVelocity.x << "|" << proVelocity.y << std::endl;
				m_World->GetComponent<Lifetime>(b).durability -= 1;
				m_World->GetComponent<Collider>(b).frameBuffer = 2;
			}
			//else {
			//	sf::Vector2f collisionNormal = circleCenter - sf::Vector2f{ closestX, closestY };
			//	float lengthSquared = collisionNormal.x * collisionNormal.x + collisionNormal.y * collisionNormal.y;
			//	if (lengthSquared == 0) return;
			//	collisionNormal /= std::sqrt(lengthSquared);
			//	float dotProd = proVelocity.x * collisionNormal.x + proVelocity.y * collisionNormal.y;
			//	proVelocity -= 2.0f * dotProd * collisionNormal;
			//}
		};

	//Obstacle -> Target Collision Logic
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

	// === Enemy === //
	//Enemy -> Obstacle Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::ObstacleBox, ColliderType::EnemyBox)] = [this](Entity obs, Entity enemy)
		{
			auto& obstacleTrans = m_World->GetComponent<Transform>(obs);
			auto& enemyTrans = m_World->GetComponent<Transform>(enemy);

			auto& obstacleRenderable = m_World->GetComponent<Renderable>(obs);
			auto& enemyRenderable = m_World->GetComponent<Renderable>(enemy);

			AABB obstacleAABB{ obstacleTrans.position, obstacleRenderable.size };
			AABB enemyAABB{ enemyTrans.position,  enemyRenderable.size };

			AABB overlap;
			if (!AABBIntersect(obstacleAABB, enemyAABB, &overlap)) return;
			if (enemyAABB.top() < obstacleAABB.top()) {
				enemyTrans.position.y -= overlap.size.y;
				m_World->GetComponent<Physics>(enemy).isGrounded = true;
			}
			else {
				// player hit obstacle from below
				enemyTrans.position.y += overlap.size.y;
			}
			// zero vertical velocity
			m_World->GetComponent<Physics>(enemy).velocity.y = 0.f;
		};

	//Enemy -> Projectile Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::ProjectileBox, ColliderType::EnemyBox)] = [this](Entity proj, Entity enemy)
		{
			Enemy& enemyComp = m_World->GetComponent<Enemy>(enemy);
			if (enemyComp.state == EnemyState::Guard) { //Add facing direction check later
				enemyComp.justDeflected = true;
				Physics& projectilePhys = m_World->GetComponent<Physics>(proj);
				projectilePhys.velocity = -projectilePhys.velocity;

				m_World->GetComponent<Collider>(proj).isDanger = true; //Set as damaging to player

				Lifetime& projLife = m_World->GetComponent<Lifetime>(proj);
				if (projLife.durability > 1) {
					projLife.durability -= 1;
					m_World->GetComponent<Renderable>(proj).tint = sf::Color::Red;
				}
			}
			else {
				m_World->GetComponent<Lifetime>(proj).durability = 0;
				m_World->GetComponent<Enemy>(enemy).health = 0;
			}
		};


	//Target -> Projectile Collision Logic
	m_CollisionHandlers[GenerateCollisionKey(ColliderType::TargetBox, ColliderType::ProjectileBox)] = [this](Entity a, Entity b)
		{
			m_World->GetComponent<Lifetime>(a).durability = 0;
			m_World->GetComponent<Lifetime>(b).durability = 0;
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