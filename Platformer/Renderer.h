#pragma once

#ifndef __RENDERER_H
#define __RENDERER_H

#include "base.h"
#include <SFML/Graphics.hpp>

//////////////////////////////////////////////////////////////////
// Class: Renderer
// - The game's renderer, handles all the game's required drawing
// - Passed a window which it will draw everything to
//////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////
class Renderer
{
public:
	Renderer(sf::RenderWindow *pWindow);
	~Renderer(void);

	void draw();
	void drawUI();

	void updateView();

	bool IsVisibleOnScreen(const sf::Vector2f& pos);
	sf::View* GetView() const;
private:
	sf::RenderWindow* pWindow;
	sf::View* pView;
};

#endif