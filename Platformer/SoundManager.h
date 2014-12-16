#pragma once

#include <SFML/Audio.hpp>
#include <map>

class SoundManager
{
public:
	SoundManager();
	~SoundManager();

	void PlayMusic(std::string file, float fVolume, bool bLoop);
	void StopMusic();
	void PauseMusic();
	void update();

	sf::Music* GetMusic() const;
private:
	sf::Music* pMusic;
};