#pragma once
#include <cstdint>
#include <bitset>
#include <memory>
#include <SFML/System/Vector2.hpp>
#define TargetFixedUpdateFrequency 144

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

struct AABB {
    sf::Vector2f pos;
    sf::Vector2f size;

    float left()   const { return pos.x; }
    float right()  const { return pos.x + size.x; }
    float top()    const { return pos.y; }
    float bottom() const { return pos.y + size.y; }
};