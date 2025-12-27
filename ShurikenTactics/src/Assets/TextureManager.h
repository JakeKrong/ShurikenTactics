#pragma once
#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>
#include "Renderable.h"

class TextureManager {
public:
	sf::Texture& Load(const std::string& filePath);

	bool ChangeEntitySprite(const std::string& textureName, Renderable& renderable, AnimationData& anim);

	void AddAnimationData(const std::string& filePath, AnimationData& animData);
	void SetAnimationData(const std::string& filePath, AnimationData& animData);
	void ResetAnimation(AnimationData& animData);

	void InitGameAnimationData();

private:
	std::unordered_map<std::string, sf::Texture> m_TextureMap;
	std::string m_TextureBasePath = "../../../../ShurikenTactics/assets/textures/";

	std::unordered_map<std::string, AnimationData> m_AnimationTextureMap;
};