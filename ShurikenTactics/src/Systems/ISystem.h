#pragma once
#include "Types.h"
#include <set>
#include <print>

class World; //Forward declaration to remove circular dependency

//Interface class to be applied by all systems
class ISystem {
public:
	virtual ~ISystem() = default;

	void AddEntity(Entity ent) {
		m_Entities.insert(ent);
	}

	void RemoveEntity(Entity ent) {
		m_Entities.erase(ent);
	}

	virtual std::set<Entity> ReturnEntities() {
		return m_Entities;
	}

	virtual void SetWorld(World* world) {
		m_World = world;
		std::println("[{} System] -> World Set!", typeid(*this).name());
	}
protected:
	std::set<Entity> m_Entities;
	World* m_World = nullptr;
};