#pragma once
#include <cstdint>
#include <bitset>
#include <memory>

using Entity = std::uint16_t;
using ComponentID = size_t;

constexpr Entity ENTITY_CAP = 500; //Arbitrary entity cap currently
constexpr ComponentID COMPONENT_CAP = 32; //Arbitrary component cap currently

using Signature = std::bitset<COMPONENT_CAP>;

template<typename T>
using Ref = std::shared_ptr<T>;

template<typename T, typename... Args>
Ref<T> CreateRef(Args&&... args) {
	return std::make_shared<T>(std::forward<Args>(args)...);
}

template<typename T>
using Scope = std::unique_ptr<T>;

template<typename T, typename... Args>
Scope<T> CreateScope(Args&&... args) {
	return std::make_unique<T>(std::forward<Args>(args)...);
}