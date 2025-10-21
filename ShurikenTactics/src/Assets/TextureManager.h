#pragma once
#include <unordered_map>
#include <SFML/Graphics/Texture.hpp>

class TextureManager {
public:
	sf::Texture& Load(const std::string& filePath);

private:
	std::unordered_map<std::string, sf::Texture> m_TextureMap;
	std::string m_TextureBasePath = "../../../../ShurikenTactics/assets/textures/";
};