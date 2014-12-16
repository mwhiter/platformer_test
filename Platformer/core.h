#pragma once

#ifndef __CORE_H
#define __CORE_H

#include "base.h"
#include "SFML/Graphics.hpp"
#include "InputController.h"
#include "Renderer.h"
#include "Entity.h"
#include "Level.h"
#include "ScreenManager.h"
#include "SoundManager.h"

//////////////////////////////////////////////////////////////////
// Class: Core
// - Contains the game core
// - Be very careful when modifying this class, as the guts of the game relies on it
//////////////////////////////////////////////////////////////////
class Core
{
public:
	Core();
	~Core(void);

	void ExitPauseScreen();

	// -----------------------------------------------------------
	// THE GUTS OF THE GAME!!!! BE VERY CAREFUL WHEN MODIFYING!!!
	void StartGame();
	void GameLoop();
	void EndGame();
	void Uninit();
	void ProcessInput();
	void Update();
	// -----------------------------------------------------------

	Level* GetCurrentLevel();

	sf::RenderWindow* GetRenderWindow();
	Renderer* GetRenderer();

	WeaponInfo* GetWeaponData() const;

	bool IsInputEvent(sf::Event &_event);

	Core& GetInstance();

	ScreenManager* GetScreenManager() const;
	SoundManager* GetSoundManager() const;
private:
	bool bExit;

	clock_t totalTimeElapsed;

	sf::RenderWindow* window;
	Renderer* renderer;
	ScreenManager* screenmgr;
	SoundManager* soundmgr;

	WeaponInfo* pWeaponData;

	Level* level;	// currently loaded level
};

// Declare an extern "Core" variable. So that the compiler doesn't complain about it being unavailable. 
extern Core core;

// Helpers
#define CORE core
#define DEBUG

// Game engine
#define MS_PER_UPDATE					(int)	16							// Time between game updates
#define UPDATES_PER_SEC					(float)	(1000 / MS_PER_UPDATE)		// How many times the game updates a second

#define ACCEL_INPUT						(float) 0.4f						// Acceleration due to input
#define ACCEL_GRAVITY					(float) 0.25f						// Acceleration due to gravity (per frame)
#define VELOCITY_MAX_X					(float) 2.0f						// The fastest we can move in the X-direction
#define TERMINAL_VELOCITY				(float) 60.0f						// The fastest we can move in the Y-direction
#define VELOCITY_JUMP					(float) -5.9f						// How fast we jump

#define CLIMB_SPEED						(float) 1.0f						// How fast we climb

#define TILE_SIZE 16

// Rendering (temporary???)
#define SCREEN_TITLE "Platformer"
#define SCREEN_WIDTH 512
#define SCREEN_HEIGHT 512

#endif