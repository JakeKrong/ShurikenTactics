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

void TextureManager::ChangeEntitySprite(const std::string& textureName, Renderable& renderable, AnimationData& anim) {
	if (renderable.texture == &this->Load(textureName)) return;
	renderable.texture = &this->Load(textureName);
	SetAnimationData(textureName, anim);
	return;
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
		animData.loopAnim = newAnimData.loopAnim;
		animData.timeSinceLastFrame = newAnimData.frameTime;
		animData.currentFrame = -1;

		//Clear animation events
		animData.animationEvents.clear();
		animData.OnAnimationEnd = nullptr;
	}
	else {
		AnimationData blankAnimData;
		animData = blankAnimData;
	}
}

void TextureManager::ResetAnimation(AnimationData& animData) {
	animData.timeSinceLastFrame = animData.frameTime;
	animData.currentFrame = -1;
	return;
}

void TextureManager::InitGameAnimationData() {
	AnimationData animData;

	// ********** Player Animations ********** //

	// Idle
	animData.spriteSheetDim = { 4,1 };
	animData.totalFrames = 4;
	animData.frameTime = 0.15f;
	AddAnimationData("Player_Idle_Sprite", animData);

	// Sprint
	animData = AnimationData{};
	animData.spriteSheetDim = { 6,1 };
	animData.totalFrames = 6;
	animData.frameTime = 0.15f;
	AddAnimationData("Player_Sprint_Sprite", animData);

	// Shuriken throw
	animData = AnimationData{};
	animData.spriteSheetDim = { 2,1 };
	animData.totalFrames = 2;
	animData.frameTime = 0.15f;
	AddAnimationData("Player_Throw_Sprite", animData);

	// Death
	animData = AnimationData{};
	animData.spriteSheetDim = { 5,1 };
	animData.totalFrames = 5;
	animData.frameTime = 0.2f;
	animData.loopAnim = false;
	AddAnimationData("Player_Death_Sprite", animData);

	// Jump
	animData = AnimationData{};
	animData.spriteSheetDim = { 4,1 };
	animData.totalFrames = 4;
	animData.frameTime = .1f;
	AddAnimationData("Player_Jump_Sprite", animData);

	
	// ********** Enemy Animations ********** //
	
	// --- Samurai --- //

	//Idle
	animData = AnimationData{};
	animData.spriteSheetDim = { 6,1 };
	animData.totalFrames = 6;
	animData.frameTime = .15f;
	AddAnimationData("Samurai/Idle", animData);

	//Walk
	animData = AnimationData{};
	animData.spriteSheetDim = { 9,1 };
	animData.totalFrames = 9;
	animData.frameTime = .15f;
	AddAnimationData("Samurai/Walk", animData);

	//Sprint
	animData = AnimationData{};
	animData.spriteSheetDim = { 8,1 };
	animData.totalFrames = 8;
	animData.frameTime = .1f;
	AddAnimationData("Samurai/Sprint", animData);

	//Attack
	animData = AnimationData{};
	animData.spriteSheetDim = { 4,1 };
	animData.totalFrames = 4;
	animData.frameTime = .15f;
	animData.loopAnim = false;
	AddAnimationData("Samurai/Attack", animData);

	animData = AnimationData{};
	//Dead
	animData.spriteSheetDim = { 6,1 };
	animData.totalFrames = 6;
	animData.frameTime = .15f;
	animData.loopAnim = false;
	AddAnimationData("Samurai/Dead", animData);

	//Guard
	animData = AnimationData{};
	animData.spriteSheetDim = { 2,1 };
	animData.totalFrames = 2;
	animData.frameTime = .2f;
	animData.loopAnim = false;
	AddAnimationData("Samurai/Guard", animData);

	// --- Archer --- //

	//Idle
	animData = AnimationData{};
	animData.spriteSheetDim = { 9,1 };
	animData.totalFrames = 9;
	animData.frameTime = .15f;
	AddAnimationData("Archer/Idle", animData);

	//Draw
	animData = AnimationData{};
	animData.spriteSheetDim = { 11,1 };
	animData.totalFrames = 11;
	animData.frameTime = .1f;
	animData.loopAnim = false;
	AddAnimationData("Archer/Draw", animData);

	//Shot
	animData = AnimationData{};
	animData.spriteSheetDim = { 3,1 };
	animData.totalFrames = 3;
	animData.frameTime = .1f;
	animData.loopAnim = false;
	AddAnimationData("Archer/Shot", animData);

	//Dead
	animData.spriteSheetDim = { 5,1 };
	animData.totalFrames = 5;
	animData.frameTime = .15f;
	animData.loopAnim = false;
	AddAnimationData("Archer/Dead", animData);

	//Arrow Break
	animData.spriteSheetDim = { 3,1 };
	animData.totalFrames = 3;
	animData.frameTime = .05f;
	animData.loopAnim = false;
	AddAnimationData("Archer/Arrow_Break", animData);

	// ********** Effects ********** //
	animData = AnimationData{};
	animData.spriteSheetDim = { 10,1 };
	animData.totalFrames = 10;
	animData.frameTime = .05f;
	animData.loopAnim = false;
	AddAnimationData("Effects/Blood", animData);
}