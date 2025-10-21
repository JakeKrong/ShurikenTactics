#include "LifetimeSystem.h"
#include "World.h"

#include "Lifetime.h"
#include <print>
#include <iostream>

void LifetimeSystem::Update(const float& deltaTime) {
	if (!m_World);

	for (Entity ent : m_Entities) {
		Lifetime& lifeComp = m_World->GetComponent<Lifetime>(ent);
		if (lifeComp.remainingTime > 0) lifeComp.remainingTime -= deltaTime;
		if (lifeComp.remainingTime <= 0 && lifeComp.durability <= 0) {
			m_EntitiesToDestroy.insert(ent);
		}
	}

	for(Entity ent : m_EntitiesToDestroy) {
		Lifetime& lifetime = m_World->GetComponent<Lifetime>(ent);
		if (lifetime.OnDestroyedFunction) {
			lifetime.OnDestroyedFunction(ent);
		}
		m_World->DestroyEntity(ent);
	}
	m_EntitiesToDestroy.clear();
}

void LifetimeSystem::SetDurability(Entity ent, const int durability) {
	if (!m_World);
	Lifetime& entLifetime = m_World->GetComponent<Lifetime>(ent);
	entLifetime.durability -= durability;

	if (entLifetime.durability <= 0 && entLifetime.remainingTime <= 0) {
		if (entLifetime.OnDestroyedFunction != nullptr) {
			entLifetime.OnDestroyedFunction(ent);
		}
		m_World->DestroyEntity(ent);
		m_EntitiesToDestroy.erase(ent);
	}
		
}