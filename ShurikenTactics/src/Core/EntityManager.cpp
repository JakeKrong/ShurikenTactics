#include "EntityManager.h"

#include <assert.h>

EntityManager::EntityManager()
{
	for (int i = 0; i < ENTITY_CAP; i++) {
		m_AvailableEntity.push(i);
	}
}

EntityManager::~EntityManager() {};

Entity EntityManager::CreateEntity() {
	assert(m_AvailableEntity.size() != 0 && "No more available Entity ID's left!");

	Entity id = m_AvailableEntity.front();
	m_AvailableEntity.pop();

	return id;
}

void EntityManager::DestroyEntity(Entity ent) {
	assert(ent < ENTITY_CAP && "Entity to be destroyed is out of range!");

	m_AvailableEntity.push(ent);
	m_Signature[ent].reset();
}

Signature& EntityManager::GetSignature(Entity ent) {
	assert(ent < ENTITY_CAP && "Entity is out of range!");
	return m_Signature[ent];
}

void EntityManager::SetSignature(Entity ent, Signature sig) {
	assert(ent < ENTITY_CAP && "Entity is out of range!");
	m_Signature[ent] = sig;
}