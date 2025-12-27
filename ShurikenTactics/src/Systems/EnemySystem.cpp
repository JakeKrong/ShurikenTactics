#include "EnemySystem.h"
#include "World.h"
#include "Components.h"

#include "TextureManager.h"
#include "SoundManager.h"
#include "TrajectoryMath.h"
#include <functional>

//Test
#include <print>
#include <iostream>

void EnemySystem::Update(const float& deltaTime) {
	if (!m_World) return;

	for (Entity ent : m_Entities) {
		Enemy& enemyComp = m_World->GetComponent<Enemy>(ent);
		Transform& enemyTrans = m_World->GetComponent<Transform>(ent);
		
		//std::cout << m_World->GetComponent<AnimationData>(ent).currentFrame << "\n";

		if (enemyComp.health <= 0) enemyComp.state = EnemyState::Dead;

		if (enemyComp.stateChangeCd > 0) enemyComp.stateChangeCd -= deltaTime;

		if (enemyComp.inPlayerKillRange && !(enemyComp.state <= EnemyState::Alerted)) {
			enemyComp.state = EnemyState::Attack;
		}

		Renderable& enemyRend = m_World->GetComponent<Renderable>(ent);
		if ((enemyComp.isFacingRight && enemyRend.flipX) || (!enemyComp.isFacingRight && !enemyRend.flipX)) {
			TurnEnemy(ent, true);
		}

		//std::println("{}", (int)enemyComp.state);
		//std::println("{}", enemyComp.stateChangeCd);

		//Switch-case to determine enemy update based on state
		switch (enemyComp.state) {
		case(EnemyState::Idle):
			UpdateIdle(ent);
			break;
		case(EnemyState::Patrol):
			//Update
			break;
		case(EnemyState::Dead):
			UpdateDead(ent);
			break;
		case(EnemyState::Alerted):
			UpdateAlert(ent);
			break;
		case(EnemyState::Chase):
			UpdateChase(ent);
			break;
		case(EnemyState::Drawing):
			UpdateDraw(ent);
			break;
		case (EnemyState::Guard):
			UpdateGuard(ent);
			break;
		case(EnemyState::Attack):
			UpdateAttack(ent);
			break;
		}

		enemyComp.inPlayerKillRange = false;
	}
}

void EnemySystem::UpdateIdle(Entity enemyEnt) {
	auto playerDataVect = m_World->GetAllComponentsOfType<Player>();
	if (playerDataVect.size() == 0) return;

	Entity playerEnt = playerDataVect[0].first;

	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);

	if (PlayerInLOS(enemyEnt) || enemyComp.inPlayerKillRange) {

		Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);
		sf::FloatRect mainEnemyCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);

		Collider& playerCol = m_World->GetComponent<Collider>(playerEnt);
		sf::FloatRect mainPlayerCol = std::get<sf::FloatRect>(playerCol.entityColliders[0].first);

		bool playerRightOfEnemy = mainPlayerCol.getCenter().x > mainEnemyCol.getCenter().x;
		if (playerRightOfEnemy != enemyComp.isFacingRight) TurnEnemy(enemyEnt);

		float newStateChangeCd;
		enemyComp.type == EnemyType::Samurai ? newStateChangeCd = .7f : newStateChangeCd = .4f;
		enemyComp.stateChangeCd = newStateChangeCd;
		enemyComp.state = EnemyState::Alerted;
		
		//Draw Exclamation Mark
		//Get the approximate coords of enemy's eye level
		sf::Vector2f losOrigin = { {mainEnemyCol.position.x + mainEnemyCol.size.x / 2},
								   {mainEnemyCol.position.y + mainEnemyCol.size.y / 5} };

		Entity exclaMark = m_World->CreateEntity();
		m_World->AddComponentToEntity<Transform>(exclaMark, { {losOrigin.x, mainEnemyCol.position.y - 30} });
		m_World->AddComponentToEntity<Renderable>(exclaMark, { {10,25}, RenderLayer::UI, &m_TextureManager->Load("Effects/Exclamation_Mark") });
		m_World->AddComponentToEntity<Lifetime>(exclaMark, { newStateChangeCd });
		m_SoundManager->PlaySound("Alert");
	}
}

void EnemySystem::UpdateAlert(Entity enemyEnt) {
	Transform& enemyTrans = m_World->GetComponent<Transform>(enemyEnt);
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Physics& enemyPhys = m_World->GetComponent<Physics>(enemyEnt);

	PlayerInLOS(enemyEnt);
	
	if (enemyComp.stateChangeCd <= 0) {
		auto playerDataVect = m_World->GetAllComponentsOfType<Player>();
		if (playerDataVect.size() == 0) return;

		if (PlayerInLOS(enemyEnt)) {
			if (enemyComp.type == EnemyType::Samurai) {
				enemyPhys.velocity.x = 0;
				if (HasWalkablePath(enemyEnt) && m_World->GetComponent<Player>(playerDataVect[0].first).playerState != PlayerState::ReadyToShoot) {
					enemyComp.state = EnemyState::Chase;
				}
				else {
					enemyComp.state = EnemyState::Guard;
					m_TextureManager->ChangeEntitySprite("Samurai/Guard", m_World->GetComponent<Renderable>(enemyEnt),
						m_World->GetComponent<AnimationData>(enemyEnt));
					enemyComp.stateChangeCd = 2;
					m_SoundManager->PlaySound("Samurai/Maere");
					sf::FloatRect mainEnemyCol = std::get<sf::FloatRect>(m_World->GetComponent<Collider>(enemyEnt).entityColliders[0].first);
					bool playerRightOfEnemy = enemyComp.lastSeenPlayerCoords.x > mainEnemyCol.getCenter().x;
					if (playerRightOfEnemy != enemyComp.isFacingRight) TurnEnemy(enemyEnt);
				}
			}
			else if (enemyComp.type == EnemyType::Archer) {
				enemyComp.state = EnemyState::Drawing;
			}
		}
		else {
			Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);
			AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);
			if (enemyComp.type == EnemyType::Samurai) {
				if (std::abs(enemyTrans.position.x - enemyComp.origin.x) > 1) {
					if (enemyPhys.velocity.x == 0) {
						m_TextureManager->ChangeEntitySprite("Samurai/Sprint", enemyRend, enemyAnim);
						bool originRightOfEnemy;
						if (enemyComp.origin.x < enemyTrans.position.x) {
							enemyPhys.velocity.x = -enemyComp.movementSpeed;
							originRightOfEnemy = false;
						}
						else {
							enemyPhys.velocity.x = enemyComp.movementSpeed;
							originRightOfEnemy = true;
						}
						if (originRightOfEnemy != enemyComp.isFacingRight) TurnEnemy(enemyEnt);
					}
				}
				else {
					enemyPhys.velocity.x = 0;
					enemyComp.state = EnemyState::Idle;
					m_TextureManager->ChangeEntitySprite("Samurai/Idle", enemyRend, enemyAnim);
					if (enemyComp.isFacingRight != enemyComp.defaultFacingRight) TurnEnemy(enemyEnt);
				}
			}
			else {
				enemyComp.state = EnemyState::Idle;
				m_TextureManager->ChangeEntitySprite("Archer/Idle", enemyRend, enemyAnim);
				if (enemyComp.isFacingRight != enemyComp.defaultFacingRight) TurnEnemy(enemyEnt);
			}
		}
	}
}

void EnemySystem::UpdateChase(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);
	Physics& enemyPhys = m_World->GetComponent<Physics>(enemyEnt);
	Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);
	AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);

	sf::FloatRect enemRectCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);

	//Start chase if not chasing
	if (std::abs(enemyPhys.velocity.x) == 0) {
		m_TextureManager->ChangeEntitySprite("Samurai/Sprint", enemyRend, enemyAnim);
		//Check if player is to the left or right of player
		if (enemyComp.lastSeenPlayerGroundedCoords.x < enemRectCol.getCenter().x) {
			enemyPhys.velocity.x = -enemyComp.movementSpeed;
		}
		else enemyPhys.velocity.x = enemyComp.movementSpeed;
	}

	bool hasLOS = PlayerInLOS(enemyEnt);

	// If not the same y-level
	if (!hasLOS || std::abs(enemyComp.lastSeenPlayerGroundedCoords.y - (enemRectCol.position.y + enemRectCol.size.y)) > 3 ||
		std::abs(enemyComp.lastSeenPlayerGroundedCoords.x - enemRectCol.getCenter().x) < 3) {
		enemyPhys.velocity.x = 0;
		enemyComp.state = EnemyState::Alerted;
		enemyComp.stateChangeCd = 0.3;
		m_TextureManager->ChangeEntitySprite("Samurai/Idle", enemyRend, enemyAnim);
	}
	else if (m_World->GetAllComponentsOfType<Player>()[0].second->playerState == PlayerState::ReadyToShoot) {
		enemyPhys.velocity.x = 0;
		enemyComp.state = EnemyState::Guard;
		m_TextureManager->ChangeEntitySprite("Samurai/Guard", enemyRend, enemyAnim);
		m_SoundManager->PlaySound("Samurai/Yare");
	}
}

void EnemySystem::UpdateGuard(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Player& playerComp = *m_World->GetAllComponentsOfType<Player>()[0].second;
	
	if (enemyComp.justDeflected) {
		enemyComp.justDeflected = false;
		enemyComp.stateChangeCd = .5f;

		Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);
		AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);

		if (enemyRend.texture != &m_TextureManager->Load("Samurai/Deflect"))
			m_TextureManager->ChangeEntitySprite("Samurai/Deflect", enemyRend, enemyAnim);
		else
			m_TextureManager->ChangeEntitySprite("Samurai/Deflect2", enemyRend, enemyAnim);
		
		Transform& enemyTrans = m_World->GetComponent<Transform>(enemyEnt);

		//Create sparks
		Entity sparks = m_World->CreateEntity();
		m_World->AddComponentToEntity<Transform>(sparks, { {enemyTrans.position.x + 30, enemyTrans.position.y - 180}, {1,1} });
		m_World->AddComponentToEntity<Renderable>(sparks, { {200,300}, RenderLayer::UI, &m_TextureManager->Load("Effects/Sparks") });
		m_World->AddComponentToEntity<AnimationData>(sparks, { {1,4}, 4, .05f});
		m_World->AddComponentToEntity<Lifetime>(sparks, {.15f});

		m_SoundManager->PlaySound("Shuriken_Deflect");
		
	}
	else {
		bool hasLos = PlayerInLOS(enemyEnt);
		if (enemyComp.stateChangeCd <= 0) {
			if (!hasLos) {
				enemyComp.state = EnemyState::Alerted;
				m_TextureManager->ChangeEntitySprite("Samurai/Idle", m_World->GetComponent<Renderable>(enemyEnt), m_World->GetComponent<AnimationData>(enemyEnt));
			}
			else if (HasWalkablePath(enemyEnt) && playerComp.playerState != PlayerState::ReadyToShoot) {
				enemyComp.state = EnemyState::Chase;
				m_SoundManager->PlaySound("Samurai/Yukuzo");
			}
			else { //Continue blocking
				m_TextureManager->ChangeEntitySprite("Samurai/Guard", m_World->GetComponent<Renderable>(enemyEnt), m_World->GetComponent<AnimationData>(enemyEnt));
				enemyComp.stateChangeCd = 0.5;

				sf::FloatRect mainEnemyCol = std::get<sf::FloatRect>(m_World->GetComponent<Collider>(enemyEnt).entityColliders[0].first);
				bool playerRightOfEnemy = enemyComp.lastSeenPlayerCoords.x > mainEnemyCol.getCenter().x;
				if (playerRightOfEnemy != enemyComp.isFacingRight && 
					std::abs(enemyComp.lastSeenPlayerCoords.x - mainEnemyCol.getCenter().x) > 10) TurnEnemy(enemyEnt);
			}
		}
		else if (hasLos && playerComp.playerState == PlayerState::ReadyToShoot) {
			enemyComp.stateChangeCd = 1.5;
		}
	}
}

void EnemySystem::UpdateAttack(Entity enemyEnt) {
	Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);

	//If not already attacking, initiate attack sequence
	if (enemyRend.texture != &m_TextureManager->Load("Samurai/Attack") ||
		enemyRend.texture != &m_TextureManager->Load("Archer/Attack")) {
		Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
		AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);
		Physics& enemyPhys = m_World->GetComponent<Physics>(enemyEnt);

		if (enemyComp.type == EnemyType::Samurai) {
			m_TextureManager->ChangeEntitySprite("Samurai/Attack", enemyRend, enemyAnim);
			enemyComp.stateChangeCd = 0;
			enemyAnim.animationEvents.insert({ 2, [&]() { m_SoundManager->PlaySound("Samurai/Sword_Slash"); } });
			enemyAnim.animationEvents.insert({ 3, [&]() { enemyComp.isLethal = true; } });
			enemyAnim.OnAnimationEnd = [&]() {
				enemyComp.isLethal = false;
				enemyComp.state = EnemyState::Alerted;
				m_TextureManager->ChangeEntitySprite("Samurai/Guard", enemyRend, enemyAnim);
				};
		}
		else if (enemyComp.type == EnemyType::Archer) {
			m_TextureManager->ChangeEntitySprite("Archer/Attack", enemyRend, enemyAnim);
			enemyComp.stateChangeCd = 0;
			enemyAnim.animationEvents.insert({ 3, [&]() { m_SoundManager->PlaySound("Samurai/Sword_Slash"); } });
			enemyAnim.animationEvents.insert({ 4, [&]() { enemyComp.isLethal = true; } });
			enemyAnim.OnAnimationEnd = [&]() {
				enemyComp.isLethal = false;
				enemyComp.state = EnemyState::Alerted;
				m_TextureManager->ChangeEntitySprite("Archer/Idle", enemyRend, enemyAnim);
				};
		}

		enemyPhys.velocity.x = 0;
	}
}

void EnemySystem::UpdateDraw(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);
	AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);
	sf::Vector2f playerCoords;

	PlayerInLOS(enemyEnt);

	//If not already drawing bow, initiate draw sequence
	if (enemyRend.texture != &m_TextureManager->Load("Archer/Draw") && 
		enemyRend.texture != &m_TextureManager->Load("Archer/Shot")) {

		m_TextureManager->ChangeEntitySprite("Archer/Draw", enemyRend, enemyAnim);
		enemyAnim.animationEvents.insert({ 7, [&]() {m_SoundManager->PlaySound("Archer/Draw_Bow"); } });

		enemyComp.stateChangeCd = 1.35; //Draw bow to fire delay interval

		//Create arrow preview
		PreviewArrow(enemyEnt);
	}
	else if (enemyComp.stateChangeCd <= 0) {
		if (enemyRend.texture == &m_TextureManager->Load("Archer/Draw")) {
			m_SoundManager->PlaySound("Archer/Shoot_Arrow");
			m_TextureManager->ChangeEntitySprite("Archer/Shot", enemyRend, enemyAnim);
			enemyComp.stateChangeCd = 1.0; //Delay after shot to state reset
			FireArrow(enemyEnt);
		}
		else if (enemyRend.texture == &m_TextureManager->Load("Archer/Shot")) {
			if (!PlayerInLOS(enemyEnt)) {
				enemyComp.state = EnemyState::Alerted;
				m_TextureManager->ChangeEntitySprite("Archer/Idle", enemyRend, enemyAnim);
			}
			else { //Draw again
				m_TextureManager->ChangeEntitySprite("Archer/Draw", m_World->GetComponent<Renderable>(enemyEnt), m_World->GetComponent<AnimationData>(enemyEnt));
				enemyAnim.animationEvents.insert({ 6, [&]() {m_SoundManager->PlaySound("Archer/Draw_Bow"); } });
				enemyComp.stateChangeCd = 1.35;
			}
		}
	}
	else if (enemyRend.texture == &m_TextureManager->Load("Archer/Draw")) {
		PreviewArrow(enemyEnt);
	}
}

void EnemySystem::UpdateDead(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);
	AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);
	if (enemyComp.type == EnemyType::Samurai) {
		m_TextureManager->ChangeEntitySprite("Samurai/Dead", enemyRend, enemyAnim);
		m_SoundManager->PlaySound("Samurai/Dead");
	}
	else if (enemyComp.type == EnemyType::Archer) {
		m_TextureManager->ChangeEntitySprite("Archer/Dead", enemyRend, enemyAnim);
		m_SoundManager->PlaySound("Archer/Dead");
	}
	
	m_World->RemoveComponentFromEntity<Enemy>(enemyEnt);
	m_World->RemoveComponentFromEntity<Physics>(enemyEnt);
	m_World->RemoveComponentFromEntity<Collider>(enemyEnt);
	m_SoundManager->PlaySound("Pierced");

	//Blood splatter
	Entity blood = m_World->CreateEntity();
	m_World->AddComponentToEntity<Transform>(blood, m_World->GetComponent<Transform>(enemyEnt));
	Renderable bloodRend{ {100,100} };
	AnimationData bloodAnim;
	bloodRend.flipX = true;
	bloodRend.layer = RenderLayer::GameObject1;
	m_TextureManager->ChangeEntitySprite("Effects/Blood", bloodRend, bloodAnim);
	m_World->AddComponentToEntity<Renderable>(blood, bloodRend);
	m_World->AddComponentToEntity<AnimationData>(blood, bloodAnim);
	m_World->AddComponentToEntity<Lifetime>(blood, { bloodAnim.totalFrames * bloodAnim.frameTime });
}

//===========================================================================================//

bool EnemySystem::PlayerInLOS(Entity enemyEnt) {
	auto playerDataVect = m_World->GetAllComponentsOfType<Player>();
	Entity playerEnt = playerDataVect[0].first;
	Player& playerComp = m_World->GetComponent<Player>(playerEnt);
	if (playerComp.health <= 0) return false;

	Transform& playerTrans = m_World->GetComponent<Transform>(playerEnt);
	Collider& playerCol = m_World->GetComponent<Collider>(playerEnt);
	sf::FloatRect mainPlayerCol = std::get<sf::FloatRect>(playerCol.entityColliders[0].first);

	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);
	sf::FloatRect mainEnemyCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);

	//Get player center
	sf::Vector2f losTarget = { {mainPlayerCol.position.x + mainPlayerCol.size.x / 2},
							   {mainPlayerCol.position.y + mainPlayerCol.size.y / 2} };
	//Get the approximate coords of enemy's eye level
	sf::Vector2f losOrigin = { {mainEnemyCol.position.x + mainEnemyCol.size.x / 2},
							   {mainEnemyCol.position.y + mainEnemyCol.size.y / 5} };

	//float fovAngle;
	//if (enemyComp.state >= EnemyState::Alerted) {
	//	fovAngle = 270;
	//}
	//else fovAngle = enemyComp.fovAngle;

	bool hasLOS = HasLineOfSight(*m_World, losOrigin, losTarget, enemyComp.fovAngle, enemyComp.isFacingRight);
	if (hasLOS) {
		enemyComp.lastSeenPlayerCoords = losTarget;
		if (m_World->GetComponent<Physics>(playerEnt).isGrounded) {
			enemyComp.lastSeenPlayerGroundedCoords = { {mainPlayerCol.position.x + (mainPlayerCol.size.x / 2)},
													   {mainPlayerCol.position.y + mainPlayerCol.size.y} };
		}
	}
		
	return hasLOS;
}

bool EnemySystem::HasWalkablePath(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);
	sf::FloatRect mainEnemyCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);

	//Get approximate player foot-level
	sf::Vector2f losTarget = { enemyComp.lastSeenPlayerGroundedCoords.x, enemyComp.lastSeenPlayerGroundedCoords.y - 5 };

	//Get approximate enemy's foot-level
	sf::Vector2f losOrigin = { {mainEnemyCol.position.x + mainEnemyCol.size.x / 2},
							   {mainEnemyCol.position.y + mainEnemyCol.size.y - 5} };
	//Check if same y-level (return false if not)
	if (std::abs(losTarget.y - losOrigin.y) > 3) return false;

	//Check if there's walkable path (no obstacles) based on enemy and players approximate foot y-level
	bool hasLOS = HasLineOfSight(*m_World, losOrigin, losTarget, enemyComp.fovAngle, enemyComp.isFacingRight);

	if (!hasLOS) return hasLOS;

	losOrigin.y = mainEnemyCol.position.y; // Get enemy's head level
	losTarget.y = losOrigin.y;			   // Set target y-level to same

	return HasLineOfSight(*m_World, losOrigin, losTarget, enemyComp.fovAngle, enemyComp.isFacingRight);
}

void EnemySystem::PreviewArrow(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);

	Entity arrowPreview = m_World->CreateEntity();

	Transform transform;
	auto& rectCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);
	transform.position = { sf::Vector2f{rectCol.getCenter().x, rectCol.getCenter().y - 30} };

	sf::Vector2f arrowDir = enemyComp.lastSeenPlayerCoords - transform.position;
	sf::Vector2f normalizedDir = arrowDir.normalized();

	transform.rotation = arrowDir.angle().asDegrees();
	m_World->AddComponentToEntity<Transform>(arrowPreview, transform);

	Renderable renderable;
	renderable.size = { 100, 10 };
	renderable.layer = RenderLayer::GameObject1;
	renderable.texture = &m_TextureManager->Load("Archer/Arrow_Preview");
	if (enemyComp.stateChangeCd <= .2f) 	renderable.tint = sf::Color::Red;

	m_World->AddComponentToEntity<Renderable>(arrowPreview, renderable);

	Lifetime lifetime;
	lifetime.remainingTime = 0.01f;
	m_World->AddComponentToEntity<Lifetime>(arrowPreview, lifetime);
}

void EnemySystem::FireArrow(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);

	Entity arrow = m_World->CreateEntity();

	Transform transform;
	auto& rectCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);
	transform.position = { sf::Vector2f{rectCol.getCenter().x, rectCol.getCenter().y - 30} };

	sf::Vector2f arrowDir = enemyComp.lastSeenPlayerCoords - transform.position;
	sf::Vector2f normalizedDir = arrowDir.normalized();

	transform.rotation = arrowDir.angle().asDegrees();
	m_World->AddComponentToEntity<Transform>(arrow, transform);

	Renderable renderable;
	renderable.size = { 100, 10 };
	renderable.layer = RenderLayer::GameObject1;
	renderable.texture = &m_TextureManager->Load("Archer/Arrow");
	m_World->AddComponentToEntity<Renderable>(arrow, renderable);

	Collider collider;
	//sf::FloatRect arrowHitbox{ transform.position, {100, 10} };
	sf::CircleShape arrowHitbox{ 5 };
	collider.entityColliders.push_back({ arrowHitbox, { normalizedDir * 85.f } });
	collider.type = ColliderType::ProjectileBox;
	collider.isDanger = true;
	collider.OnCollision = [&](Entity arrow) {m_SoundManager->PlaySound("Archer/Arrow_Impact"); };
	m_World->AddComponentToEntity<Collider>(arrow, collider);

	Physics physics;
	physics.velocity = { normalizedDir.x * 1700, normalizedDir.y * 1700 };
	m_World->AddComponentToEntity<Physics>(arrow, physics);

	Lifetime lifetime;
	lifetime.durability = 1;
	lifetime.OnDestroyedFunction = [&](Entity arrow){
		Transform& arrowTrans = m_World->GetComponent<Transform>(arrow);
		//Create arrow break effect
		Entity brokenArrow = m_World->CreateEntity();
		m_World->AddComponentToEntity<Transform>(brokenArrow, m_World->GetComponent<Transform>(arrow));
		m_World->AddComponentToEntity<Renderable>(brokenArrow, m_World->GetComponent<Renderable>(arrow));
		m_World->AddComponentToEntity<AnimationData>(brokenArrow, {});
		Renderable& newRend = m_World->GetComponent<Renderable>(brokenArrow);
		AnimationData& newAnim = m_World->GetComponent<AnimationData>(brokenArrow);
		m_TextureManager->ChangeEntitySprite("Archer/Arrow_Break", newRend, newAnim);
		newRend.size.y  = newRend.size.y * 1.7f;
		m_World->AddComponentToEntity<Lifetime>(brokenArrow, { newAnim.totalFrames * newAnim.frameTime});
		};
	m_World->AddComponentToEntity<Lifetime>(arrow, lifetime);
}

void EnemySystem::TurnEnemy(Entity enemyEnt, bool syncDir) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);
	Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);
	auto& [colShape, colOffset] = enemyCol.entityColliders[0];

	if (!syncDir) {
		enemyComp.isFacingRight = !enemyComp.isFacingRight;
		enemyComp.isFacingRight ? enemyRend.flipX = false : enemyRend.flipX = true;
		colOffset.x = enemyRend.size.x - std::get<sf::FloatRect>(colShape).size.x - colOffset.x;
	}
	else {
		enemyComp.isFacingRight ? enemyRend.flipX = false : enemyRend.flipX = true;
		colOffset.x = enemyRend.size.x - std::get<sf::FloatRect>(colShape).size.x - colOffset.x;
	}
}

void EnemySystem::SetTextureManager(TextureManager* textureManager) {
	m_TextureManager = textureManager;
	return;
}

void EnemySystem::SetSoundManager(SoundManager* soundManager) {
	m_SoundManager = soundManager;
	return;
}

int EnemySystem::getEnemyCount() {
	return m_Entities.size();
}