#pragma once

enum class EnemyState : uint8_t{
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
	Attack
};

enum class EnemyType : uint8_t {
	Samurai,
	Archer
};

struct Enemy {
	//Default type and state
	sf::Vector2f origin{};
	sf::Vector2f lastSeenPlayerCoords;
	sf::Vector2f lastSeenPlayerGroundedCoords;
	
	int health = 1;
	int fovAngle = 140;
	float stateChangeCd = 0;
	float movementSpeed = 500.f;
	float jumpForce = 1000;

	EnemyType type = EnemyType::Samurai;
	EnemyState state = EnemyState::Idle;
	
	bool isFacingRight = true;
	bool defaultFacingRight = true;
	bool inPlayerKillRange = false;
	bool isLethal = false;
	bool justDeflected = false;

	//sf::Vector2f currentPlatform;
	//bool playerInFOV = false;
};