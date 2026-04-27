#pragma once

enum class PlayerState : uint8_t{
	Idle,
	Walking,
	Airborne,
	Aiming,
	ReadyToShoot,
	Throwing,
	Dead
};

struct Player {
	float health{ 100 };
	float movement{ 100 };

	float movementSpeed = 20.0f;
	float jumpForce = 1000.f;
	PlayerState playerState = PlayerState::Idle;

	bool isControllable{ false };
};