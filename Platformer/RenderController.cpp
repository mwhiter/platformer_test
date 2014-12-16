#include "PCH.h"

RenderController::RenderController(Entity* pEntity):
	Component(pEntity)
{
}


RenderController::~RenderController(void)
{
}

void RenderController::update()
{

}

void RenderController::draw()
{
	DisplacementController* pDisplacement = pEntity->GetDisplacementController();
	if (pDisplacement == NULL) return;

	sf::RectangleShape shape(sf::Vector2f(pEntity->GetSizeX(), pEntity->GetSizeY()));

	shape.setPosition(pEntity->GetLeftEdgeX(), pEntity->GetTopEdgeY());

	sf::Color color;

	if (pEntity->IsHostile()) color = sf::Color::Red;
	else color = sf::Color::White;

	shape.setFillColor(color);
	core.GetRenderWindow()->draw(shape);
}