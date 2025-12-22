#pragma once

enum class EnemyState {
	Idle,
	Patrol,
	Dead,
	Alerted,
	Chase,
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
	bool isFacingRight = true;
	int fovAngle = 90;

	sf::Vector2f origin{};
	bool defaultFacingRight = true;
	bool inPlayerKillRange = false;
	bool isLethal = false;
	bool justDeflected = false;

	int health = 1;
	float stateChangeCd = 0;
	float movementSpeed = 500.f;
	float jumpForce = 1000;

	sf::Vector2f lastSeenPlayerCoords;
	sf::Vector2f lastSeenPlayerGroundedCoords;
	//sf::Vector2f currentPlatform;
	//bool playerInFOV = false;
};