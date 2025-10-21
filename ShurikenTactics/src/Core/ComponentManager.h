#pragma once
#include <cstdint>
#include <string>
#include <typeindex>
#include <vector>
#include <unordered_map>
#include <cassert>
//#include <typeindex>
#include "Types.h"

//Interface for ComponentArray
class IComponentArray {
public:
	virtual ~IComponentArray() = default;
	virtual void EntityDestroyed(Entity ent) = 0;
};

//ComponentArray class created for each component registered
template <typename T>
class ComponentArray: public IComponentArray {
public:
	ComponentArray() {
		m_ComponentArray.resize(ENTITY_CAP);
	}

	void AddComponent(Entity ent, T comp) {
		assert(m_EntityToIndexMap.find(ent) == m_EntityToIndexMap.end() && "Component already added into entity once!");

		m_EntityToIndexMap[ent] = m_Index;
		m_IndexToEntityMap[m_Index] = ent;
		m_ComponentArray[m_Index] = comp;

		m_Index++;
	}

	void RemoveComponent(Entity ent) {
		assert(m_EntityToIndexMap.find(ent) != m_EntityToIndexMap.end() && "Removing non-existent component!");

		//Swap the index of removed component with the last component in the vector and maps
		size_t RemovedEntityIndex = m_EntityToIndexMap[ent];
		size_t LastEntityIndex = m_Index - 1;
		m_ComponentArray[RemovedEntityIndex] = m_ComponentArray[LastEntityIndex];

		Entity lastMapEntity = m_IndexToEntityMap[LastEntityIndex];
		m_EntityToIndexMap[lastMapEntity] = RemovedEntityIndex;
		m_IndexToEntityMap[RemovedEntityIndex] = lastMapEntity;

		m_EntityToIndexMap.erase(ent);
		m_IndexToEntityMap.erase(LastEntityIndex);

		m_Index--;
	}

	T& GetComponentData(Entity ent) {
		assert(m_EntityToIndexMap.find(ent) != m_EntityToIndexMap.end() && "Retrieving a non-existent component!");
		return m_ComponentArray[m_EntityToIndexMap[ent]];
	}

	void EntityDestroyed(Entity ent) override {
		if (m_EntityToIndexMap.find(ent) != m_EntityToIndexMap.end()) {
			RemoveComponent(ent);
		}
	}

	std::vector<std::pair<Entity,T*>> GetAllComponents(){
		std::vector<std::pair<Entity,T*>> componentVect;
		componentVect.reserve(m_Index + 1);
		for (auto& [ent, idx] : m_EntityToIndexMap) {
			componentVect.push_back({ ent, &m_ComponentArray[idx] });
		}
		return componentVect;
	}

private:
	std::vector<T> m_ComponentArray;
	std::unordered_map<std::size_t,Entity> m_IndexToEntityMap;
	std::unordered_map<Entity, std::size_t> m_EntityToIndexMap;
	size_t m_Index = 0;
};

class ComponentManager {
public:
	ComponentManager() : m_CurrComponentId(0)
	{}

	template<typename T>
	void RegisterComponent() {
		//std::type_index typeInd(typeid(T));
		std::type_index componentType = typeid(T);

		assert(m_ComponentIDs.find(componentType) == m_ComponentIDs.end() && "Registering an already registered component!");

		m_ComponentIDs.insert({ componentType , m_CurrComponentId });
		m_ComponentArraysMap.insert({ componentType, CreateRef<ComponentArray<T>>() });

		m_CurrComponentId++;
	}

	template <typename T>
	ComponentID GetComponentID() {
		std::type_index componentType = typeid(T);
		assert(m_ComponentIDs.find(componentType) != m_ComponentIDs.end() && "Searching ID for Component Type is Not Registered!");

		return m_ComponentIDs[componentType];
	}

	//template<typename T>
	//void AddComponent(Entity ent, T comp) {
	//	std::type_index componentType = typeid(T);
	//	assert(m_ComponentIDs.find(componentType) != m_ComponentIDs.end() && "Adding a Component Type is that is Not Registered!");

	//	m_ComponentArraysMap[componentType]->AddComponent(ent, comp);
	//}

	template<typename T, typename... Args>
	void AddComponent(Entity ent, Args&&... args) {
		std::type_index componentType = typeid(T);
		assert(m_ComponentIDs.find(componentType) != m_ComponentIDs.end() && "Adding a Component Type is that is Not Registered!");

		GetComponentArray<T>()->AddComponent(ent, args...);
	}

	template<typename T>
	void RemoveComponent(Entity ent) {
		std::type_index componentType = typeid(T);
		assert(m_ComponentIDs.find(componentType) != m_ComponentIDs.end() && "Removing a Component Type is that is Not Registered!");

		m_ComponentArraysMap[componentType]->RemoveComponent(ent);
	}

	void EntityDestroyed(Entity ent) {
		for (auto [compType, compArray] : m_ComponentArraysMap) {
			compArray->EntityDestroyed(ent);
		}
	}

	template<typename T>
	Ref<ComponentArray<T>> GetComponentArray() {
		std::type_index componentType = typeid(T);
		assert(m_ComponentArraysMap.find(componentType) != m_ComponentArraysMap.end() && "Trying to Get a Non-Registered Component!");

		return std::static_pointer_cast<ComponentArray<T>>(m_ComponentArraysMap[componentType]);
	}

	template<typename T>
	T& GetComponentData(Entity ent) {
		std::type_index componentType = typeid(T);
		assert(m_ComponentArraysMap.find(componentType) != m_ComponentArraysMap.end() && "Trying to Get a Non-Registered Component!");
		return GetComponentArray<T>()->GetComponentData(ent);
	}

private:
	std::unordered_map<std::type_index, ComponentID> m_ComponentIDs{};
	std::unordered_map<std::type_index, Ref<IComponentArray>> m_ComponentArraysMap{};
	ComponentID m_CurrComponentId;
};