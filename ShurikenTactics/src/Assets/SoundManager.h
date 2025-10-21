#pragma once
#include <unordered_map>
#include <deque>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundSource.hpp>

class SoundManager {
public:
	sf::Sound& Load(const std::string& path);

	void CleanUpStopped();
	void StopAll();

private:
	std::unordered_map<std::string, sf::SoundBuffer> m_BuffersMap;
	std::deque<sf::Sound> m_SoundQueue;

	std::string m_SoundBasePath = "../../../../ShurikenTactics/assets/sound/";
};