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

		if (enemyComp.inPlayerKillRange) enemyComp.state = EnemyState::Attack;

		//Switch-case to determine enemy update based on state
		switch (enemyComp.state) {
		case(EnemyState::Idle):
			UpdateIdle(ent);
			break;
		case(EnemyState::Patrol):
			//Update
			break;
		case(EnemyState::Alerted):
			UpdateAlert(ent);
			break;
		case(EnemyState::Chase):
			//Update
			break;
		case(EnemyState::Drawing):
			//Update
			break;
		case (EnemyState::Guard):
		case (EnemyState::Deflect):
			UpdateGuard(ent);
			break;
		case(EnemyState::Attack):
			UpdateAttack(ent);
			break;
		case(EnemyState::Dead):
			UpdateDead(ent);
			break;
		}

		enemyComp.inPlayerKillRange = false;
	}
}

void EnemySystem::UpdateIdle(Entity enemyEnt) {
	auto playerDataVect = m_World->GetAllComponentsOfType<Player>();
	if (playerDataVect.size() == 0) return;

	//Can add angle limitations later
	if (PlayerInLOS(enemyEnt)) {
		Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);

		float newStateChangeCd = .7f;
		enemyComp.stateChangeCd = newStateChangeCd;
		enemyComp.state = EnemyState::Alerted;
		
		//Draw Exclamation Mark
		Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);
		sf::FloatRect mainEnemyCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);

		//Get the approximate coords of enemy's eye level
		sf::Vector2f losOrigin = { {mainEnemyCol.position.x + mainEnemyCol.size.x / 2},
								   {mainEnemyCol.position.y + mainEnemyCol.size.y / 5} };

		Entity exclaMark = m_World->CreateEntity();
		m_World->AddComponentToEntity<Transform>(exclaMark, { {losOrigin.x, mainEnemyCol.position.y - 30} });
		m_World->AddComponentToEntity<Renderable>(exclaMark, { {10,25}, RenderLayer::UI, &m_TextureManager->Load("Effects/Exclamation_Mark") });
		m_World->AddComponentToEntity<Lifetime>(exclaMark, { newStateChangeCd });
		m_SoundManager->Load("Alert");
	}
}

void EnemySystem::UpdateAlert(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	if (enemyComp.stateChangeCd <= 0) {
		auto playerDataVect = m_World->GetAllComponentsOfType<Player>();
		if (playerDataVect.size() == 0) return;

		//Can add angle limitations later
		if (PlayerInLOS(enemyEnt)) {
			//1. (If not on same axis || player is certain distance away) && player.isAiming
			// Change to --> Guard + parry
			enemyComp.state = EnemyState::Guard;
			m_TextureManager->ChangeEntitySprite("Samurai/Guard", m_World->GetComponent<Renderable>(enemyEnt),
				m_World->GetComponent<AnimationData>(enemyEnt));
			enemyComp.stateChangeCd = 2;
			//2. For now, check if on same y-axis, if yes, walk towards (future can add jumping)
			// Change to --> Chase


		}
		//3. If neither, go back to idle
		else enemyComp.state = EnemyState::Idle;
	}
}

void EnemySystem::UpdateGuard(Entity enemyEnt) {
	Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
	
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

		m_SoundManager->Load("Shuriken_Deflect");
		
	}
	else if (enemyComp.stateChangeCd <= 0) {
		if (!PlayerInLOS(enemyEnt)) { //Player not aiming || not in line of sight 
			enemyComp.state = EnemyState::Alerted;
			m_TextureManager->ChangeEntitySprite("Samurai/Idle", m_World->GetComponent<Renderable>(enemyEnt), m_World->GetComponent<AnimationData>(enemyEnt));
		}
		else { //Continue blocking
			m_TextureManager->ChangeEntitySprite("Samurai/Guard", m_World->GetComponent<Renderable>(enemyEnt), m_World->GetComponent<AnimationData>(enemyEnt));
			enemyComp.stateChangeCd = 2;
		}
	}
}

void EnemySystem::UpdateAttack(Entity enemyEnt) {
	Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);

	//If not already attacking, initiate attack sequence
	if (enemyRend.texture != &m_TextureManager->Load("Samurai/Attack")) {
		Enemy& enemyComp = m_World->GetComponent<Enemy>(enemyEnt);
		AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);

		m_TextureManager->ChangeEntitySprite("Samurai/Attack", enemyRend, enemyAnim);
		enemyComp.stateChangeCd = 0;
		enemyAnim.animationEvents.insert({ 3, [&]() {enemyComp.isLethal = true; } });
		enemyAnim.OnAnimationEnd = [&]() { 
			enemyComp.isLethal = false; 
			enemyComp.state = EnemyState::Alerted;
			m_TextureManager->ChangeEntitySprite("Samurai/Guard", enemyRend, enemyAnim);
		};
	}
}

void EnemySystem::UpdateDead(Entity enemyEnt) {
	Renderable& enemyRend = m_World->GetComponent<Renderable>(enemyEnt);
	AnimationData& enemyAnim = m_World->GetComponent<AnimationData>(enemyEnt);
	m_TextureManager->ChangeEntitySprite("Samurai/Dead", enemyRend, enemyAnim);
	m_World->RemoveComponentFromEntity<Enemy>(enemyEnt);
	m_World->RemoveComponentFromEntity<Physics>(enemyEnt);
	m_World->RemoveComponentFromEntity<Collider>(enemyEnt);
	m_SoundManager->Load("Pierced");

	//Blood splatter
	Entity blood = m_World->CreateEntity();
	m_World->AddComponentToEntity<Transform>(blood, m_World->GetComponent<Transform>(enemyEnt));
	Renderable bloodRend{ {100,100} };
	AnimationData bloodAnim;
	bloodRend.flipX = true;
	m_TextureManager->ChangeEntitySprite("Effects/Blood", bloodRend, bloodAnim);
	m_World->AddComponentToEntity<Renderable>(blood, bloodRend);
	m_World->AddComponentToEntity<AnimationData>(blood, bloodAnim);
	m_World->AddComponentToEntity<Lifetime>(blood, { bloodAnim.totalFrames * bloodAnim.frameTime });
}

// ===========================================================================================//

bool EnemySystem::PlayerInLOS(Entity enemyEnt) {
	auto playerDataVect = m_World->GetAllComponentsOfType<Player>();
	Entity playerEnt = playerDataVect[0].first;
	Transform& playerTrans = m_World->GetComponent<Transform>(playerEnt);
	Collider& playerCol = m_World->GetComponent<Collider>(playerEnt);
	sf::FloatRect mainPlayerCol = std::get<sf::FloatRect>(playerCol.entityColliders[0].first);

	Collider& enemyCol = m_World->GetComponent<Collider>(enemyEnt);
	sf::FloatRect mainEnemyCol = std::get<sf::FloatRect>(enemyCol.entityColliders[0].first);

	//Get player center
	sf::Vector2f losTarget = { {mainPlayerCol.position.x + mainPlayerCol.size.x / 2},
							   {mainPlayerCol.position.y + mainPlayerCol.size.y / 2} };
	//Get the approximate coords of enemy's eye level
	sf::Vector2f losOrigin = { {mainEnemyCol.position.x + mainEnemyCol.size.x / 2},
							   {mainEnemyCol.position.y + mainEnemyCol.size.y / 5} };

	return HasLineOfSight(*m_World, losOrigin, losTarget);
}


void EnemySystem::SetTextureManager(TextureManager* textureManager) {
	m_TextureManager = textureManager;
	return;
}


void EnemySystem::SetSoundManager(SoundManager* soundManager) {
	m_SoundManager = soundManager;
	return;
}