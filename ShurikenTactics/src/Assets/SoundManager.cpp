#include "Types.h"
#include "SoundManager.h"
#include <print>

sf::SoundBuffer& SoundManager::LoadBuffer(const std::string& filePath) {
	if (m_BuffersMap.count(filePath)) {
		return m_BuffersMap[filePath];
	}
	else {
		sf::SoundBuffer buffer;
		if (!buffer.loadFromFile(m_SoundBasePath + filePath + ".mp3")) {
			std::println("[Sound Manager] Failed to load audio from {}!", filePath);
		}
		auto [iter, _] = m_BuffersMap.emplace(filePath, std::move(buffer));
		return iter->second;
	}
}

SoundID SoundManager::PlaySound(const std::string& filePath, bool loop) {
	sf::Sound sound(LoadBuffer(filePath));
	SoundID newId = m_NextId++;
	m_SoundQueue.emplace_back(SoundData{ newId, sound });
	m_SoundQueue.back().sound.setVolume(GlobalVolumeLevel);
	m_SoundQueue.back().sound.setLooping(loop);
	m_SoundQueue.back().sound.play();
	return newId;
}

void SoundManager::StopSound(SoundID id) {
	for (auto& sData : m_SoundQueue) {
		if (sData.id == id) {
			sData.sound.stop();
			break;
		}
	}
}

void SoundManager::StopAllSound() {
	for (auto& sData : m_SoundQueue) {
		sData.sound.stop();
	}
}

void SoundManager::CleanUpStopped()
{
	std::erase_if(m_SoundQueue, [](const SoundData& s) { return s.sound.getStatus() == sf::Sound::Status::Stopped; });
}

void SoundManager::PlayMusic(const std::string& filePath, bool loop) {
	m_Music.stop();
	if (!m_Music.openFromFile(m_SoundBasePath + filePath + ".mp3")) {
		std::println("[Sound Manager] Failed to load music from {}!", filePath);
		return;
	}
	m_Music.setVolume(GlobalVolumeLevel);
	m_Music.setLooping(loop);
	m_Music.play();
}

void SoundManager::SetMusicVolume(int volume) {
	m_Music.setVolume(volume);
}

void SoundManager::StopMusic() {
	m_Music.stop();
}