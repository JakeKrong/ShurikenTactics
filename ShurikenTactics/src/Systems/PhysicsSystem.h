#pragma once
#include "ISystem.h"

class PhysicsSystem : public ISystem {
public:
	void Update(const float& deltaTime);
};