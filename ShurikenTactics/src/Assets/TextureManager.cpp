#include "TextureManager.h"
#include <print>

sf::Texture& TextureManager::Load(const std::string& filePath) {
	auto it = m_TextureMap.find(filePath);

	//If texture already loaded, return texture data
	if (it != m_TextureMap.end()) {
		return it->second;
	}

	sf::Texture texture;
	if (!texture.loadFromFile(m_TextureBasePath + filePath)) {
		std::println("[Texture Manager] Failed to load texture from {}!", filePath);
	}

	return m_TextureMap[filePath] = std::move(texture);
}