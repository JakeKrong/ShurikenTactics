#pragma once
#include "Types.h"
#include <vector>
#include <print>

class World; //Forward declaration to remove circular dependency

//Interface class to be applied by all systems
class ISystem {
public:
	virtual ~ISystem() = default;

	void AddEntity(Entity ent) {
		if (std::find(m_Entities.begin(), m_Entities.end(), ent) == m_Entities.end())
			m_Entities.emplace_back(ent);
	}

	void RemoveEntity(Entity ent) {
		std::erase(m_Entities, ent);
	}

	virtual std::vector<Entity> ReturnEntities() {
		return m_Entities;
	}

	virtual void SetWorld(World* world) {
		m_World = world;
		//std::println("[{} System] -> World Set!", typeid(*this).name());
	}
protected:
	std::vector<Entity> m_Entities;
	World* m_World = nullptr;
};