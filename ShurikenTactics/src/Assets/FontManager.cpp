#include "FontManager.h"
#include "print"

sf::Font& FontManager::Load(const std::string& fontName) {
	if (m_FontMap.find(fontName) == m_FontMap.end()) {
		sf::Font font;
		if (!font.openFromFile(fontBasePath + fontName + ".ttf"))
			std::println("[Font Manager] Unable to load font '{}'!", fontName);
		m_FontMap.emplace(fontName, font);
	}
	return m_FontMap[fontName];
}