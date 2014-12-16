#include "Screen.h"
#include "enums.h"

Screen::Screen(ScreenType eScreenType) :
	m_eScreenType(eScreenType),
	pElements()
{
}

Screen::~Screen()
{
	for (std::vector<UIElement*>::iterator it = pElements.begin(); it != pElements.end(); ++it)
	{
		delete (*it);
	}

	pElements.clear();
}

// Adds an element to the screen
UIElement* Screen::AddElement(UIElement* pElement)
{
	if (pElement == NULL) return NULL;

	pElements.push_back(pElement);
	return pElement;
}

std::vector<UIElement*> Screen::GetUIElements() const
{
	return pElements;
}

// Loop through all the UIElements on the screen and process the event
void Screen::ProcessInput(sf::Event& _event)
{
	for (std::vector<UIElement*>::iterator it = pElements.begin(); it != pElements.end(); ++it)
	{
		// If a mouse event is registered on a button, we must immediately return on this frame: no other action can be registered
		// Why? Because some mouse events might destroy the screen (switch to a new one).
		if ((*it)->MouseEvent(_event))
			return;
	}
}

// Loop through all the UIElements on the screen and draw them
void Screen::draw()
{
	for (std::vector<UIElement*>::iterator it = pElements.begin(); it != pElements.end(); ++it)
	{
		(*it)->draw();
	}
}

ScreenType Screen::GetType()
{
	return m_eScreenType;
}