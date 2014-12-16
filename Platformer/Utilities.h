#pragma once

#include "PCH.h"

// Quadtree class
// Stores a list of entities (to-do, template???)
class Quadtree
{
public:
	Quadtree(int level, sf::Rect<int> bounds) :
		level(level),
		bounds(bounds)
	{
		nodes.resize(4);
	}

	// Clear the quadtree
	void clear()
	{
		objects.clear();

		for (std::vector<Quadtree*>::size_type i = 0; i < nodes.size(); i++)
		{
			if (nodes[i] != NULL)
			{
				nodes[i] = NULL;
			}
		}
	}

	// Split the quadtree into 4 equal sections
	void split()
	{
		int subWidth = (int)(bounds.width / 2);
		int subHeight = (int)(bounds.height / 2);
		int x = (int)bounds.left;
		int y = (int)bounds.top;

		nodes[0] = new Quadtree(level + 1, sf::Rect<int>(x + subWidth, y, subWidth, subHeight));
		nodes[1] = new Quadtree(level + 1, sf::Rect<int>(x, y, subWidth, subHeight));
		nodes[2] = new Quadtree(level + 1, sf::Rect<int>(x, y + subHeight, subWidth, subHeight));
		nodes[3] = new Quadtree(level + 1, sf::Rect<int>(x + subWidth, y + subHeight, subWidth, subHeight));
	}

	// Find out the node which an object belongs to
	int getIndex(const sf::Rect<int>& rect)
	{
		int index = -1;
		double vertMidpoint = bounds.left + (bounds.width / 2);
		double horizMidpoint = bounds.top + (bounds.height / 2);
		
		// Can the object completely fit in the top or bottom quadrant?
		bool topQuadrant = (rect.top < horizMidpoint && rect.top + rect.height < horizMidpoint);
		bool bottomQuadrant = (rect.top > horizMidpoint);
		bool leftQuadrant = (rect.left < vertMidpoint && rect.left + rect.width < vertMidpoint);
		bool rightQuadrant = rect.left > vertMidpoint;

		if (leftQuadrant)
		{
			if (topQuadrant)
				index = 1;
			else if (bottomQuadrant)
				index = 2;
		}
		else
		{
			if (topQuadrant)
				index = 0;
			else if (bottomQuadrant)
				index = 3;
		}

		return index;
	}

	// Insert into the quadtree
	void insert(Entity* pEntity)
	{
#pragma warning( disable : 4244 )
		sf::Rect<int> rect((int)pEntity->GetLeftEdgeX(), (int)pEntity->GetTopEdgeY(), pEntity->GetSizeX(), pEntity->GetSizeY());
#pragma warning( default : 4244 )
		if (nodes[0] != NULL)
		{
			int index = getIndex(rect);

			if (index != -1)
			{
				nodes[index]->insert(pEntity);

				return;
			}
		}

		objects.push_back(pEntity);

		if (objects.size() > MAX_OBJECTS && level < MAX_LEVELS)
		{
			if (nodes[0] == NULL)
			{
				split();
			}

			std::vector<Entity*>::size_type i = 0;
			while (i < objects.size())
			{
				Entity* pLoopEntity = objects[i];
#pragma warning( disable : 4244 )
				sf::Rect<int> loopRect((int)pLoopEntity->GetLeftEdgeX(), (int)pLoopEntity->GetTopEdgeY(), pLoopEntity->GetSizeX(), pLoopEntity->GetSizeY());
#pragma warning( default : 4244 )
				int index = getIndex(loopRect);
				if (index != -1)
				{
					objects.erase(objects.begin() + i);
					nodes[index]->insert(pLoopEntity);
				}
				else
				{
					i++;
				}
			}
		}
	}

	// Return all objects that can collide with pEntity
	std::vector<Entity*>& retrieve(std::vector<Entity*>& returnObjects, Entity* pEntity)
	{
#pragma warning( disable : 4244 )
		int index = getIndex(sf::Rect<int>((int)pEntity->GetLeftEdgeX(), (int)pEntity->GetTopEdgeY(), pEntity->GetSizeX(), pEntity->GetSizeY()));
#pragma warning( default : 4244 )
		if (index != -1 && nodes[0] != NULL)
		{
			nodes[index]->retrieve(returnObjects, pEntity);
		}

		for (std::vector<Entity*>::size_type i = 0; i < objects.size(); i++)
		{
			returnObjects.push_back(objects[i]);
		}

		return returnObjects;
	}
private:
	int level;
	sf::Rect<int> bounds;
	std::vector<Entity*> objects;
	std::vector<Quadtree*> nodes;

	const uint MAX_OBJECTS = 15;
	const int MAX_LEVELS = 5;
};