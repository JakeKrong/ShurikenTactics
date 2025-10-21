#pragma once
#include<unordered_map>
#include <string>
#include <SFML/Graphics/Font.hpp>

class FontManager {
public:
	sf::Font& Load(const std::string& fontName);
private:
	std::unordered_map<std::string, sf::Font> m_FontMap;
	std::string fontBasePath{ "C:/Windows/Fonts/" };
};