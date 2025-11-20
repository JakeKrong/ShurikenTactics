#pragma once
#include "ISystem.h"
#include <set>

class LifetimeSystem : public ISystem {
public:
	void Update(const float& deltaTime);
	void SetDurability(Entity ent, const int durability);

private:
	std::set<Entity> m_EntitiesToDestroy;
};