#include "PCH.h"
#include "ScreenManager.h"

// Wrapper functions to CORE
// These are global functions, outside the scope of everything, so go through CORE for this
void ClickStartGameButton()			{ CORE.StartGame(); }
void ClickExitButton()				{ CORE.EndGame(); }
void Pause_ClickResumeButton()		{ CORE.GetScreenManager()->GoToScreenType(SCREEN_TYPE_INGAME); }
void Pause_ClickMainMenuButton()	{ CORE.GetCurrentLevel()->EndLevel(); }

void StartMainMenu()				{ CORE.GetSoundManager()->PlayMusic("music_menu.wav", 40.0f, true); }
void StartPause()					{ CORE.GetSoundManager()->PauseMusic(); }
void EndPause()						{ CORE.GetSoundManager()->GetMusic()->play(); }
void StartInGame()					{ CORE.GetSoundManager()->PlayMusic("music_ingame.wav", 40.0f, true); }
void StartLoading()
{
	CORE.GetSoundManager()->StopMusic();
}

ScreenManager::ScreenManager():
	EnterCallbackFunc(NULL),
	EndCallbackFunc(NULL),
	screens()
{
}


ScreenManager::~ScreenManager()
{
}

// Add a new screen
Screen* ScreenManager::AddScreen(ScreenType eScreenType)
{
	Screen* pScreen = new Screen(eScreenType);
	if (pScreen)
	{
		screens.push(pScreen);
		PopulateScreen(screens.top(), eScreenType);
		if (EnterCallbackFunc)
			EnterCallbackFunc();

	}
	return pScreen;
}

// Delete the top-most screen
void ScreenManager::DeleteActiveScreen()
{
	if (EndCallbackFunc)
		EndCallbackFunc();

	Screen* pScreen = screens.top();
	screens.pop();
	delete pScreen;
}

Screen* ScreenManager::GetActiveScreen() const
{
	return screens.top();
}

// Populate the screen with pre-defined elements
void ScreenManager::PopulateScreen(Screen* pScreen,  ScreenType eScreenType)
{
	if (eScreenType == NO_SCREEN_TYPE) return;
	if (pScreen == NULL) return;

	Button* pButton1 = NULL;
	Button* pButton2 = NULL;
	Button* pButton3 = NULL;

	switch (eScreenType)
	{
	case SCREEN_TYPE_INGAME:
		SetEnterCallbackFunc(StartInGame);
		break;
	case SCREEN_TYPE_MAIN_MENU:
		pButton1 = (Button*)pScreen->AddElement(new Button("Start Game!", sf::Vector2f(SCREEN_WIDTH / 2 - 70, 200), sf::Vector2f(140, 50)));
		pButton2 = (Button*)pScreen->AddElement(new Button("Exit", sf::Vector2f(SCREEN_WIDTH / 2 - 70, 260), sf::Vector2f(140, 50)));
		pButton3 = (Button*)pScreen->AddElement(new Button("My Title", sf::Vector2f(SCREEN_WIDTH / 2 - 70, 80), sf::Vector2f(140, 50)));

		pButton1->GetText()->setColor(sf::Color::Black);
		pButton2->GetText()->setColor(sf::Color::Black);
		pButton3->GetText()->setColor(sf::Color::White);

		pButton1->GetText()->setCharacterSize(20);
		pButton2->GetText()->setCharacterSize(20);
		pButton3->GetText()->setCharacterSize(36);

		pButton1->SetColor(sf::Color::Yellow);
		pButton2->SetColor(sf::Color::Yellow);
		pButton3->SetColor(sf::Color(0,0,0,0));

		pButton1->SetCallbackFunc(&ClickStartGameButton);
		pButton2->SetCallbackFunc(&ClickExitButton);

		SetEnterCallbackFunc(StartMainMenu);
		break;
	case SCREEN_TYPE_PAUSE:
		pButton1 = (Button*)pScreen->AddElement(new Button("Resume Game", sf::Vector2f(SCREEN_WIDTH / 2 - 70, 180), sf::Vector2f(180, 50)));
		pButton2 = (Button*)pScreen->AddElement(new Button("Exit to Main Menu", sf::Vector2f(SCREEN_WIDTH / 2 - 70, 260), sf::Vector2f(180, 50)));

		pButton1->GetText()->setColor(sf::Color::Black);
		pButton2->GetText()->setColor(sf::Color::Black);

		pButton2->GetText()->setCharacterSize(20);
		pButton1->GetText()->setCharacterSize(20);

		pButton1->SetColor(sf::Color::Yellow);
		pButton2->SetColor(sf::Color::Yellow);

		pButton1->SetCallbackFunc(&Pause_ClickResumeButton);
		pButton2->SetCallbackFunc(&Pause_ClickMainMenuButton);

		SetEnterCallbackFunc(StartPause);
		SetEndCallbackFunc(EndPause);
		break;
	case SCREEN_TYPE_LOADING:
		SetEnterCallbackFunc(StartLoading);
		pButton1 = (Button*)pScreen->AddElement(new Button("Loading...", sf::Vector2f(SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 - 25), sf::Vector2f(100, 50)));
		
		pButton1->GetText()->setCharacterSize(24);
		
		pButton1->SetColor(sf::Color(0, 0, 0, 0));
		break;
	default:
		return;
		break;
	}
}

// Go to the first screen of type eScreenType
void ScreenManager::GoToScreenType(ScreenType eScreenType)
{
	// keep popping off screens until we find eScreenType...if not, then just exit the game
	while (!screens.empty())
	{
		Screen* pTopScreen = screens.top();
		if (pTopScreen->GetType() == eScreenType)
		{
			return;
		}
		else
			DeleteActiveScreen();
	}

	// Something really bad happened if we got here!
	core.EndGame();
}

void ScreenManager::SetEnterCallbackFunc(void(*callback)())
{
	EnterCallbackFunc = callback;
}

void ScreenManager::SetEndCallbackFunc(void(*callback)())
{
	EndCallbackFunc = callback;
}