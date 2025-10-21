#pragma once
#include <queue>
#include <array>
#include <cstdint>

#include "Types.h"

class EntityManager {
public:
	EntityManager();
	~EntityManager();

	Entity CreateEntity();
	void DestroyEntity(Entity ent);
	Signature& GetSignature(Entity entity);
	void SetSignature(Entity ent, Signature sig);

private:
	std::queue<Entity> m_AvailableEntity{};
	std::array<Signature, ENTITY_CAP> m_Signature{};
};