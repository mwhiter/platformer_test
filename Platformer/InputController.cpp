#include "PCH.h"

InputController::InputController(Entity* pEntity):
	Component(pEntity)
{
}


InputController::~InputController(void)
{
}

void InputController::update()
{

}

// Received some input from the window - what do we do with it?
void InputController::update(sf::Event _event)
{
	if(pEntity == NULL) return;

	// Determine the event that was passed in to the window
	switch(_event.type)
	{
	case sf::Event::KeyPressed:
		ProcessKeyboardInput(_event.key.code, true);
		break;
	case sf::Event::KeyReleased:
		ProcessKeyboardInput(_event.key.code, false);
		break;
	case sf::Event::MouseButtonPressed:
		ProcessMouseInput(_event.mouseButton.button, true);
		break;
	case sf::Event::MouseButtonReleased:
		ProcessMouseInput(_event.mouseButton.button, false);
		break;
	}
}

// Determine what happens when the keyboard is pressed
void InputController::ProcessKeyboardInput(sf::Keyboard::Key key, bool bPressed)
{
	// Key was pressed
	if(bPressed)
	{
		// Hit the escape key
		if (key == sf::Keyboard::Escape)
		{
			Screen* pActiveScreen = CORE.GetScreenManager()->GetActiveScreen();
			if (pActiveScreen->GetType() == SCREEN_TYPE_INGAME)
			{
				CORE.GetScreenManager()->AddScreen(SCREEN_TYPE_PAUSE);
			}
		}

		// MOVEMENT - these controls only work if we have movement
		if (pEntity->GetMoveController() != NULL && pEntity->GetDisplacementController() != NULL)
		{
			switch (key)
			{
			case sf::Keyboard::A:
				pEntity->GetMoveController()->move(DIRECTION_LEFT);
				break;
			case sf::Keyboard::D:
				pEntity->GetMoveController()->move(DIRECTION_RIGHT);
				break;
			case sf::Keyboard::S:
				pEntity->GetMoveController()->move(DIRECTION_DOWN);
				break;
			case sf::Keyboard::W:
				pEntity->GetMoveController()->move(DIRECTION_UP);
				break;
			case sf::Keyboard::Space:
				pEntity->GetMoveController()->jump();
				break;
			case sf::Keyboard::LControl:
			case sf::Keyboard::RControl:
				pEntity->GetMoveController()->crouch();
				break;
			}
		}
		// WEAPONS - these controls only work if we have a weapon
		if (pEntity->GetWeaponController() != NULL)
		{
			switch (key)
			{
			case sf::Keyboard::R:
				pEntity->GetWeaponController()->GetActiveWeapon()->startReload();
				break;
			case sf::Keyboard::Num1:
				pEntity->GetWeaponController()->SetActiveWeapon(0);
				break;
			case sf::Keyboard::Num2:
				pEntity->GetWeaponController()->SetActiveWeapon(1);
				break;
			case sf::Keyboard::Num3:
				pEntity->GetWeaponController()->SetActiveWeapon(2);
				break;
			case sf::Keyboard::Num4:
				pEntity->GetWeaponController()->SetActiveWeapon(3);
				break;
			case sf::Keyboard::Num5:
				pEntity->GetWeaponController()->SetActiveWeapon(4);
				break;
			}
		}
	}
	// Key was released
	else
	{
		// MOVEMENT - these controls only work if we have movement
		if (pEntity->GetMoveController() != NULL && pEntity->GetDisplacementController() != NULL)
		{
			switch (key)
			{
			case sf::Keyboard::A:
				pEntity->GetMoveController()->stop(DIRECTION_LEFT);
				break;
			case sf::Keyboard::D:
				pEntity->GetMoveController()->stop(DIRECTION_RIGHT);
				break;
			case sf::Keyboard::S:
				pEntity->GetMoveController()->stop(DIRECTION_DOWN);
				break;
			case sf::Keyboard::W:
				pEntity->GetMoveController()->stop(DIRECTION_UP);
				break;
			case sf::Keyboard::LControl:
			case sf::Keyboard::RControl:
				pEntity->GetMoveController()->stand();
				break;
			}
		}
	}
}

void InputController::ProcessMouseInput(sf::Mouse::Button button, bool bPressed)
{
	// Mouse Button was pressed
	if (bPressed)
	{
		// WEAPON - these controllers will only work if we have weapons
		if (pEntity->GetWeaponController() != NULL)
		{
			switch (button)
			{
			case sf::Mouse::Left:
				pEntity->SetFireState(Entity::FIRE_STATE_SHOOT_PRIMARY);
				break;
			case sf::Mouse::Right:
				break;
			}
		}
	}
	// Mouse Button was released
	else
	{
		// WEAPON - these controllers will only work if we have weapons
		if (pEntity->GetWeaponController() != NULL)
		{
			switch (button)
			{
			case sf::Mouse::Left:
				pEntity->SetFireState(Entity::NO_FIRE_STATE);
				break;
			case sf::Mouse::Right:
				break;
			}
		}
	}
}