#include "PCH.h"

// does entity #1 collide with entity #2?
bool IsCollide(Entity* pEntity, Entity* pOtherEntity)
{
	if (pEntity == NULL) return false;
	if (pOtherEntity == NULL) return false;
	if (pEntity == pOtherEntity) return true;

	sf::Rect<float> rect1(pEntity->GetLeftEdgeX(), pEntity->GetTopEdgeY(), pEntity->GetSizeX(), pEntity->GetSizeY());
	sf::Rect<float> rect2(pOtherEntity->GetLeftEdgeX(), pOtherEntity->GetTopEdgeY(), pOtherEntity->GetSizeX(), pOtherEntity->GetSizeY());

	return rect1.intersects(rect2);
}