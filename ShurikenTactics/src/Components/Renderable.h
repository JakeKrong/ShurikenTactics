#pragma once
#include <functional>
#include <SFML/System/Vector2.hpp>
#include<SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include "Transform.h"

//const float DEFAULT_SIZE = 30.0f;
//sf::Color DEFAULT_COLOR{ sf::Color::White };

enum class RenderLayer {
	Background,
	GameObject2,
	Player,
	GameObject1,
	UI
};

struct Renderable {
	sf::Vector2f size{ 10.0f, 10.0f };
	RenderLayer layer{0};
	sf::Texture* texture = nullptr;
	bool visible{ true };
	bool flipX = false;
	sf::Color tint = sf::Color::White;
	std::function<void(Transform&)> transformFunction{}; //For transform related display mods (rotation, shaking, etc...)
};

struct AnimationData {
	sf::Vector2u spriteSheetDim{ 1,1 };		//Declare sprite sheet layout, x * y (Program reads sprite sheet from left to right, top to bottom)
	int totalFrames = 1;					//Total frames included in sheet 
	float frameTime = 1.0f;					//Time each frame should be played for
	bool loopAnim = true;

	int currentFrame = 0;
	float timeSinceLastFrame = 999.0f;
	sf::IntRect activeSprite{};
	std::unordered_map<int, std::function<void()>> animationEvents;
	std::function<void()> OnAnimationEnd;
};