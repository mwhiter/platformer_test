#pragma once

#ifndef __LEVEL_H
#define __LEVEL_H

#include "Tile.h"
#include "src/Tmx.h"
#include "Utilities.h"

//////////////////////////////////////////////////////////////////
// Class: Level
// - Stores information about a level in the game
// - Each level is made up of tiles
//////////////////////////////////////////////////////////////////
class Level
{
public:
	struct StartPosition
	{
		bool active;
		sf::Vector2f pos;
	};

	Level();
	~Level(void);

	bool LoadLevel(const std::string& filepath);
	void EndLevel();

	void UnloadLevel();

	int GetTileWidth() const;
	int GetTileHeight() const;

	float GetWidth() const;
	float GetHeight() const;

	Tile* GetTile(int iX, int iY) const;
	
	void moveToPosition(Entity* pEntity, const StartPosition& resetPos) const;

	void processInput(sf::Event _event);
	void draw();
	void update();

	bool IsLevelLoaded() const;

	// Level Entities
	std::vector<Entity*>* GetEntities();
	std::vector<StartPosition> GetStartPositions() const;

	Entity* addEntity(sf::Vector2f size, sf::Uint32 components);
	void removeEntity(uint iIndex);
	Entity* GetEntity(uint iIndex) const;
	Entity* GetPlayer() const;
private:
	void Init();
	void UnInit();

	void LoadStartPositions();
	void LoadObjects();
	void LoadPlayer();

	Tmx::Map* pMap;
	bool bLoaded;

	std::vector<StartPosition> m_startPositions;

	// Quadtree useful for collision detection
	Quadtree m_quad;

	// Level grid. Changed when the level is loaded
	std::vector< std::vector<Tile*> > m_grid;

	// Entities on the level
	std::vector<Entity*> entities;

	// Special vector to assist in efficient (unordered) array deletions.
	std::vector<int> viDeleteIndex;
};

#define GET_PLAYER()					GetPlayer()

#endif