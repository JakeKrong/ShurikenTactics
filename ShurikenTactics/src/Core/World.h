#pragma once

#include "Types.h"
#include "EntityManager.h"
#include "ComponentManager.h"
#include "SystemManager.h"

class World {
public:
	World();
	World(World&& other) noexcept;
	World(const World& other) = delete;

	// *** Entity Functions *** //
	Entity CreateEntity();
	void DestroyEntity(Entity);

	// *** Component Functions *** //
	template<typename T>
	void RegisterComponent() {
		m_ComponentManager->RegisterComponent<T>();
	}

	template<typename T>
	void AddComponentToEntity(Entity ent, T comp) {
		m_ComponentManager->AddComponent<T>(ent, comp);

		auto& updatedSig = m_EntityManager->GetSignature(ent);
		updatedSig.set(m_ComponentManager->GetComponentID<T>(), true); //Set the signature bit (based on component type's ID to trues)

		m_EntityManager->SetSignature(ent, updatedSig);
		m_SystemManager->EntitySignatureChanged(ent, updatedSig);
	}
	template<typename T>
	void RemoveComponentFromEntity(Entity ent, T comp) {
		m_ComponentManager->RemoveComponent<T>(ent);

		auto& updatedSig = m_EntityManager->GetSignature(ent);
		updatedSig.set(m_ComponentManager->GetComponentID<T>(), false); //Set the signature bit (based on component type's ID to trues)

		m_EntityManager->SetSignature(ent, updatedSig);
		m_SystemManager->EntitySignatureChanged(ent, updatedSig);
	}

	template<typename T>
	ComponentID GetComponentID() {
		return m_ComponentManager->GetComponentID<T>();
	}

	template<typename T>
	T& GetComponent(Entity ent) {
		return m_ComponentManager->GetComponentData<T>(ent);
	}

	template<typename T>
	bool HasComponent(Entity entity) {
		try {
			m_ComponentManager->GetComponentData<T>(entity);
			return true;
		}
		catch (...) {
			return false;
		}
	}

	// *** System Functions *** //
	template<typename T>
	Ref<T> RegisterSystem() {
		return m_SystemManager->RegisterSystem<T>();
	}

	template<typename T>
	void SetSystemSignature(Signature sig) {
		m_SystemManager->SetSignature<T>(sig);
	}

	// *** Reset Function *** //
	void ResetManagers();

private:
	Scope<EntityManager> m_EntityManager;
	Scope<ComponentManager> m_ComponentManager;
	Scope<SystemManager> m_SystemManager;
};