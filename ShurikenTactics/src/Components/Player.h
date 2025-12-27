#pragma once

enum PlayerState {
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
	bool isControllable{ false };

	float movementSpeed = 20.0f;
	float jumpForce = 1000.f;
	PlayerState playerState = PlayerState::Idle;
};