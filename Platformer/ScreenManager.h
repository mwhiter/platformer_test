#pragma once

#include <stack>
#include "Screen.h"

// Screen manager class for the game. Will add new screens
class ScreenManager
{
public:
	ScreenManager();
	~ScreenManager();

	Screen* GetActiveScreen() const;

	Screen* AddScreen(ScreenType eScreenType);
	void DeleteActiveScreen();

	void PopulateScreen(Screen* screen, ScreenType eScreenType);
	void GoToScreenType(ScreenType eScreenType);

	void SetEnterCallbackFunc(void(*callback)());
	void SetEndCallbackFunc(void(*callback)());
private:
	std::stack<Screen*> screens;
	void(*EnterCallbackFunc)();	// called when the screen starts
	void(*EndCallbackFunc)();	// called when the screen dies
};

