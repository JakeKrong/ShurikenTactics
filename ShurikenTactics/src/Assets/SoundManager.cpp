#include "SoundManager.h"
#include <print>

sf::Sound& SoundManager::Load(const std::string& filePath) {
	if (!m_BuffersMap.count(filePath)) {
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(m_SoundBasePath + filePath)) {
			std::println("[Sound Manager] Failed to load texture from {}!", filePath);
		}
		m_BuffersMap[filePath] = std::move(buffer);
	}

	m_SoundQueue.emplace_back(m_BuffersMap[filePath]);
	return m_SoundQueue.back();
}

void SoundManager::CleanUpStopped() {
	//Can consider adding a lock guard

	while (!m_SoundQueue.empty() && m_SoundQueue.front().getStatus() == sf::SoundSource::Status::Stopped) {
		m_SoundQueue.pop_front();
	}
}

void SoundManager::StopAll() {
	for (sf::Sound& audio : m_SoundQueue) {
		audio.stop();
	}
}