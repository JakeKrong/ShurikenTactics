#pragma once
#include <unordered_map>
#include <deque>
#include <string>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Audio/SoundSource.hpp>
#include <SFML/Audio/Music.hpp>

// Forward declare or include your types if SoundID is defined there
#include "Types.h" 

class SoundManager {
public:
    SoundID PlaySound(const std::string& name, bool loop = false);
    void StopSound(SoundID id);
    void StopAllSound();
    void CleanUpStopped();

    void PlayMusic(const std::string& name, bool loop = true);
    void SetMusicVolume(int volume);
    void StopMusic();

    // void setVolume(float volume); // Implement later if needed

private:
    struct SoundData {
        SoundID id;
        sf::Sound sound;
    };

    sf::SoundBuffer& LoadBuffer(const std::string& name);

private:
    std::unordered_map<std::string, sf::SoundBuffer> m_BuffersMap;
    std::deque<SoundData> m_SoundQueue;

    // Ensure this path is correct relative to your .exe
    std::string m_SoundBasePath = "../../../../ShurikenTactics/assets/sound/";

    SoundID m_NextId = 1;
    sf::Music m_Music;
};