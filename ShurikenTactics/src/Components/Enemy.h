#pragma once

enum class EnemyState {
	Idle,
	Patrol,
	Alerted,
	Chase,
	Dead,
	//Archer
	Drawing,
	Shoot,
	//Samurai
	Guard,
	Deflect,
	Attack
};

enum class EnemyType {
	Samurai,
	Archer
};

struct Enemy {
	//Default type and state
	EnemyType type = EnemyType::Samurai; 
	EnemyState state = EnemyState::Idle;
	sf::Vector2f origin;

	bool inPlayerKillRange = false;
	bool isLethal = false;
	bool justDeflected = false;


	sf::Vector2f currentPlatform;
	sf::Vector2f lastSeenPlayerCoords;
	//bool playerInFOV = false;
	int health = 1;
	float stateChangeCd = 0;
	float movementSpeed = 500.f;
	float jumpForce = 1000;
};