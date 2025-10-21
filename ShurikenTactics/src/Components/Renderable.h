#pragma once
#include <functional>
#include <SFML/System/Vector2.hpp>
#include<SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>

//const float DEFAULT_SIZE = 30.0f;
//sf::Color DEFAULT_COLOR{ sf::Color::White };

struct Renderable {
	sf::Vector2f size{ 10.0f, 10.0f };
	bool visible{ true };
	
	sf::Texture* texture = nullptr;
	bool animated = false;
	sf::Vector2u spriteSheetDim {1,1} ;
	int totalFrames = 1;
	float frameTime = 1.0f;
	bool flipX = false;
	std::function<void(sf::Transform&)> transformFunc{};

	float timeSinceLastFrame = 0.0f;
	int currentFrame = 0;
	sf::IntRect activeSprite{};
};