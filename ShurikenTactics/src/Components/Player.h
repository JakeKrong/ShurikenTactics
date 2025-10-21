#pragma once

struct Player {
	float health{ 100 };
	float movement{ 100 };
	bool isControllable{ false };

	float movementSpeed = 20.0f;
};