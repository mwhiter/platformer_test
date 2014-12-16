#pragma once

#ifndef SCREEN_H
#define SCREEN_H

#include "UIElement.h"
#include "enums.h"

class Screen
{
public:
	Screen(ScreenType eScreenType);
	~Screen();

	UIElement* AddElement(UIElement* pElement);

	std::vector<UIElement*> GetUIElements() const;

	ScreenType GetType();

	void ProcessInput(sf::Event& _event);

	void draw();
private:
	ScreenType m_eScreenType;
	std::vector<UIElement*> pElements;
};

#endif