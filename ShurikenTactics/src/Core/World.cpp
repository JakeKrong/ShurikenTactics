#include "World.h"
#include <print>

World::World() :
	m_EntityManager(CreateScope<EntityManager>()),
	m_ComponentManager(CreateScope<ComponentManager>()),
	m_SystemManager(CreateScope<SystemManager>())
{ }

//Move Constructor
World::World(World&& other) noexcept:
	m_EntityManager(std::move(other.m_EntityManager)),
	m_ComponentManager(std::move(other.m_ComponentManager)),
	m_SystemManager(std::move(other.m_SystemManager))
{}

Entity World::CreateEntity() {
	return m_EntityManager->CreateEntity();
}

void World::DestroyEntity(Entity ent) {
	m_EntityManager->DestroyEntity(ent);
	m_ComponentManager->EntityDestroyed(ent);
	m_SystemManager->EntityDestroyed(ent);
}

void World::ResetManagers() {
	m_EntityManager.reset(new EntityManager);
	m_ComponentManager.reset(new ComponentManager);
	m_SystemManager.reset(new SystemManager);
}