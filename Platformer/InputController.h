#pragma once

#ifndef __INPUT_CONTROLLER_H
#define __INPUT_CONTROLLER_H

#include "Entity.h"
#include "Component.h"

// Forward declaration
class Entity;

class InputController : public Component
{
public:
	InputController(Entity* pEntity);
	~InputController(void);

	void update();
	void update(sf::Event _event);
private:
	// Private member functions
	void ProcessKeyboardInput(sf::Keyboard::Key key, bool bPressed);
	void ProcessMouseInput(sf::Mouse::Button button, bool bPressed);
};

#endif