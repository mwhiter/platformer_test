#include "PCH.h"

Core core;

Core::Core():
	bExit(false),
	totalTimeElapsed(0),
	window(new sf::RenderWindow(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), SCREEN_TITLE, sf::Style::Titlebar)),
	renderer(new Renderer(window)),
	level(new Level()),
	screenmgr(new ScreenManager()),
	soundmgr(new SoundManager()),
	pWeaponData(new WeaponInfo[5]
	{
		WeaponInfo("M1911 Colt", "m1911_shoot.wav", WEAPON_TYPE_SEMI_AUTO, 80, 7, 49, 6, 35, 1600),
		WeaponInfo("M1 Garand", "m1garand_shoot.wav", WEAPON_TYPE_SEMI_AUTO, 60, 8, 64, 10, 45, 2400),
		WeaponInfo("Thompson", "thompson_shoot.wav", WEAPON_TYPE_FULL_AUTO, 600, 20, 200, 15, 15, 2700),
		WeaponInfo("M1918 BAR", "", WEAPON_TYPE_FULL_AUTO, 500, 20, 140, 12, 20, 4000),
		WeaponInfo("Shotgun", "", WEAPON_TYPE_SEMI_AUTO, 15, 5, 20, 6, 750, 6000)
	})
{
}

Core::~Core(void)
{
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////	
void Core::StartGame()
{
	// Start a loading screen whenever we want to load a level...
	CORE.GetScreenManager()->AddScreen(SCREEN_TYPE_LOADING);
	// force a redraw here...
	renderer->draw();
	
	bool bSuccessful = level->LoadLevel("../assets/maps/test.tmx");
	
	// End the loading screen (and return to the main menu screen) after we're done
	CORE.GetScreenManager()->GoToScreenType(SCREEN_TYPE_MAIN_MENU);

	// And finally go to in-game if we're successful. (if not we'll be at the main menu with a known error state)
	if(bSuccessful) screenmgr->AddScreen(SCREEN_TYPE_INGAME);
}

void Core::GameLoop()
{
	clock_t currentTime = clock();				// Record the initial clock lifetime
	clock_t accumulator = 0;					// Helps keep track of time elapsed

	// Game loop!
	while (!bExit)
	{
		// Calculate the time between frames (and store it)
		clock_t newTime = clock();
		clock_t frameTime = newTime - currentTime;
		currentTime = newTime;

		// Add the frame time to the accumulator, in case we need to catch up (because it took too long to render a frame?
		accumulator += frameTime;

		ProcessInput();

		// Update the game world. Can occur multiple times if the game is rendering slow enough!
		while (accumulator >= MS_PER_UPDATE)
		{
			Update();
			
			// Time-keeping, to make sure we don't infinite loop here
			accumulator -= MS_PER_UPDATE;
			totalTimeElapsed += MS_PER_UPDATE;
		}

		// Drawing of the game
		renderer->draw();
	}

	CORE.Uninit();
}

void Core::EndGame()
{
	bExit = true;
}

// Unload any assets and kill the program correctly
void Core::Uninit()
{
	window->close();
}

// Process program input
void Core::ProcessInput()
{
	// Process events in the window
	sf::Event _event;
	while(window->pollEvent(_event))
	{
		// Screen always processes input
		screenmgr->GetActiveScreen()->ProcessInput(_event);

		// Level listens to input only if an in-game screen is active
		if (screenmgr->GetActiveScreen()->GetType() == SCREEN_TYPE_INGAME)
		{
			if (level->IsLevelLoaded())
				level->processInput(_event);
		}
	}
}

// Update the game world
void Core::Update()
{
	soundmgr->update();

	// Only do if we're in the game
	if (screenmgr->GetActiveScreen()->GetType() == SCREEN_TYPE_INGAME)
	{
		// Update the level.
		if (level)
			level->update();
	}
}

////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////
////////////////////////////////////////////////////////

// Checks to see if the event involves input (and therefore entities should listen to it)
bool Core::IsInputEvent(sf::Event &_event)
{
	switch(_event.type)
	{
	case sf::Event::KeyPressed:
	case sf::Event::KeyReleased:
	case sf::Event::MouseButtonPressed:
	case sf::Event::MouseButtonReleased:
		return true;
		break;
	default:
		return false;
		break;
	}
}

WeaponInfo* Core::GetWeaponData() const
{
	return pWeaponData;
}

sf::RenderWindow* Core::GetRenderWindow()
{
	return window;
}

Renderer* Core::GetRenderer()
{
	return renderer;
}

Core& Core::GetInstance()
{
	return core;
}

Level* Core::GetCurrentLevel()
{
	return level;
}

ScreenManager* Core::GetScreenManager() const
{
	return screenmgr;
}

SoundManager* Core::GetSoundManager() const
{
	return soundmgr;
}