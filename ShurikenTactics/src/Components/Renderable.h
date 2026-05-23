#pragma once
#include <functional>
#include <SFML/System/Vector2.hpp>
#include<SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Text.hpp>
#include "Transform.h"
#include <memory>

enum class RenderLayer : uint8_t{
	Background,
	GameObject2,
	Player,
	GameObject1,
	UI
};

struct Renderable {
	sf::Vector2f size{ 10.0f, 10.0f };
	sf::Texture* texture = nullptr;
	std::unique_ptr<sf::Text> text = nullptr;
	RenderLayer layer{ 0 };
	bool visible{ true };
	bool flipX = false;
	sf::Color tint = sf::Color::White;
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