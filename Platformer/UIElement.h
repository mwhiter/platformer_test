#pragma once

#include <SFML/Graphics.hpp>

#ifndef UI_ELEMENT_H
#define UI_ELEMENT_H

class UIElement
{
public:
	UIElement(float x, float y, float sx, float sy);
	UIElement(sf::Vector2f pos, sf::Vector2f size);

	void SetSize(float x, float y);
	void SetPosition(float x, float y);

	virtual void draw() = 0;
	virtual bool MouseEvent(sf::Event& _event) = 0;

	bool IsIntersection(int x, int y);

	// Callback function to do something when an action is triggered
	void SetCallbackFunc(void(*callback)());
protected:
	// Function pointer - first we tell the button what button to do, then do something with it
	void(*CallbackFunc)();
	sf::Vector2f pos;
	sf::Vector2f size;
};

class Button : public UIElement
{
public:
	Button(std::string szString, float x, float y, float sx, float sy);
	Button(std::string szString, sf::Vector2f pos, sf::Vector2f size);
	~Button();

	sf::Text* GetText() const;
	void SetColor(sf::Color color);
	
	bool MouseEvent(sf::Event& _event);

	void draw();
private:
	sf::Text* text;
	std::string szString;
	sf::Color color;
};

#endif // UI_ELEMENT_H