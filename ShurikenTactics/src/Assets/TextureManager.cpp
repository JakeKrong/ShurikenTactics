#include "TextureManager.h"
#include <print>

sf::Texture& TextureManager::Load(const std::string& filePath) {
	auto it = m_TextureMap.find(filePath);

	//If texture already loaded, return texture data
	if (it != m_TextureMap.end()) {
		return it->second;
	}

	sf::Texture texture;
	if (!texture.loadFromFile(m_TextureBasePath + filePath + ".png")) {
		std::println("[Texture Manager] Failed to load texture from {}!", filePath);
	}

	return m_TextureMap[filePath] = std::move(texture);
}

void TextureManager::AddAnimationData(const std::string& filePath, AnimationData& animData) {
	m_AnimationTextureMap[filePath] = std::move(animData);
}

void TextureManager::SetAnimationData(const std::string& filePath, AnimationData& animData) {
	if (m_AnimationTextureMap.find(filePath) != m_AnimationTextureMap.end()) {
		AnimationData newAnimData = m_AnimationTextureMap[filePath];
		animData.spriteSheetDim = newAnimData.spriteSheetDim;
		animData.totalFrames = newAnimData.totalFrames;
		animData.frameTime = newAnimData.frameTime;
		animData.timeSinceLastFrame = newAnimData.frameTime;
	}
}

void TextureManager::InitGameAnimationData() {
	AnimationData animData;

	// *** Player Animations *** //

	// Idle
	animData.spriteSheetDim = { 4,1 };
	animData.totalFrames = 4;
	animData.frameTime = 0.15f;
	AddAnimationData("Player_Idle_Sprite", animData);

	// Idle
	animData.spriteSheetDim = { 6,1 };
	animData.totalFrames = 6;
	animData.frameTime = 0.15f;
	AddAnimationData("Player_Sprint_Sprite", animData);

	// Shuriken throw
	animData.spriteSheetDim = { 2,1 };
	animData.totalFrames = 2;
	animData.frameTime = 0.15f;
	AddAnimationData("Player_Throw_Sprite", animData);
}