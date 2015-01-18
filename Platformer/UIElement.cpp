#include "UIElement.h"
#include "PCH.h"

/////////////////////////////////////
// UIElement
/////////////////////////////////////

UIElement::UIElement(float x, float y, float sx, float sy):
	pos(sf::Vector2f(x,y)),
	size(sf::Vector2f(x,y))
{
	CallbackFunc = NULL;
}

UIElement::UIElement(sf::Vector2f pos, sf::Vector2f size):
	pos(pos),
	size(size)
{
	CallbackFunc = NULL;
}

bool UIElement::IsIntersection(int x, int y)
{
	if (x >= pos.x && x <= pos.x + size.x && y >= pos.y && y <= pos.y + size.y) return true;

	return false;
}

void UIElement::SetSize(float x, float y)
{
	size.x = x;
	size.y = y;
}

void UIElement::SetPosition(float x, float y)
{
	pos.x = x;
	pos.y = y;
}

// Callback function
void UIElement::SetCallbackFunc(void(*callback)())
{
	CallbackFunc = callback;
}

Button::Button(std::string szString, float x, float y, float sx, float sy) :
	UIElement(x, y, sx, sy),
	szString(szString),
	text(new sf::Text()),
	color()
{

}

Button::Button(std::string szString, sf::Vector2f pos, sf::Vector2f size) :
	UIElement(pos, size),
	szString(szString),
	text(new sf::Text()),
	color()
{

}

Button::~Button()
{
}

void Button::SetColor(sf::Color newColor)
{
	color = newColor;
}

sf::Text* Button::GetText() const
{
	return text;
}

void Button::draw()
{
	sf::Font font;
	font.loadFromFile("../assets/fonts/Harabara Mais.otf");
	text->setFont(font);

	text->setString(szString);

	sf::RectangleShape shape(size);
	shape.setPosition(pos);
	shape.setFillColor(color);

	// center text
	sf::FloatRect textRect = text->getLocalBounds();
	text->setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
	text->setPosition(pos.x + (size.x / 2), pos.y + (size.y / 2));

	core.GetRenderWindow()->draw(shape);
	core.GetRenderWindow()->draw(*text);
}

// Returns true if the mouse event was successfully executed
bool Button::MouseEvent(sf::Event& _event)
{
	// Left-mouse button is clicked
	if (_event.type == sf::Event::EventType::MouseButtonPressed && _event.key.code == sf::Mouse::Button::Left)
	{
		if (IsIntersection(_event.mouseButton.x, _event.mouseButton.y))
		{
			// If we hooked a callback function, then call that function
			if (CallbackFunc != NULL)
			{
				CallbackFunc();
				return true;
			}
		}
	}

	return false;
}