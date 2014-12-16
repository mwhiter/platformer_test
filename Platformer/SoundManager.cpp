#include "SoundManager.h"
#include "PCH.h"

SoundManager::SoundManager():
	pMusic(new sf::Music())
{
}


SoundManager::~SoundManager()
{
}

void SoundManager::PlayMusic(std::string file, float fVolume, bool bLoop)
{
	if (pMusic->getStatus() == sf::Music::Playing)
	{
		pMusic->stop();
		delete pMusic;
		pMusic = new sf::Music();
	}

	if (!pMusic->openFromFile("../assets/music/" + file))
		return;

	pMusic->setVolume(fVolume);
	pMusic->setLoop(true);
	pMusic->play();
}

void SoundManager::StopMusic()
{
	pMusic->stop();
}

void SoundManager::PauseMusic()
{
	pMusic->pause();
}

sf::Music* SoundManager::GetMusic() const
{
	return pMusic;
}

void SoundManager::update()
{
	pMusic->setPosition(CORE.GetRenderer()->GetView()->getCenter().x, CORE.GetRenderer()->GetView()->getCenter().y, 0);
}