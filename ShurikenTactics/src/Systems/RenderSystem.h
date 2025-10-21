#pragma once
#include <SFML/Graphics/RenderWindow.hpp>
#include "Renderable.h"
#include "ISystem.h"

class RenderSystem : public ISystem {
public:
	void Update(sf::RenderWindow&, const float&);
	void SetAnimationFrame(Renderable&, const float&);
	void SetWorld(World*);
};