#pragma once
#include <string>
#include <unordered_map>
#include <typeindex>

#include <cassert>

#include "Types.h"
#include "ISystem.h"

class SystemManager {
public:
	template<typename T>
	Ref<T> RegisterSystem() {
		std::type_index systemName = typeid(T);
		assert(m_SystemsMap.find(systemName) == m_SystemsMap.end() && "Registering an already registered system!");

		auto registeredSystem = CreateRef<T>();
		m_SystemsMap.insert({ systemName, registeredSystem });
		m_SignaturesMap.insert({ systemName, Signature{} });

		return registeredSystem;
	}

	template<typename T>
	void SetSignature(const Signature& sig) {
		std::type_index systemName = typeid(T);
		assert(m_SignaturesMap.find(systemName) != m_SignaturesMap.end() && "Setting signature for a non-registered system!");

		m_SignaturesMap[systemName] = sig;
	}

	inline void EntityDestroyed(Entity ent) {
		for (const auto&[sysName, sysPoint] : m_SystemsMap) {
			sysPoint->RemoveEntity(ent);
		}
	}

	//To be called to reassign entity to systems based on their new signature
	inline void EntitySignatureChanged(Entity ent, Signature newSig) {
		for (auto&[sysName, sysPoint] : m_SystemsMap) {
			const Signature& systemSig = m_SignaturesMap[sysName];
			
			if ((systemSig & newSig) == systemSig) { //Comparison of signatures using bitwise &
				sysPoint->AddEntity(ent);
			}
			else {
				sysPoint->RemoveEntity(ent);
			}
		}
	}

private:
	std::unordered_map<std::type_index, Signature> m_SignaturesMap{};
	std::unordered_map<std::type_index, Ref<ISystem>> m_SystemsMap{};
};