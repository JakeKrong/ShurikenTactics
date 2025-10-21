#pragma once
#include <functional>

struct Lifetime {
	float remainingTime{ 0.0f };
	int durability{ 1 };

	std::function<void(Entity ent)> OnDestroyedFunction = nullptr;
};