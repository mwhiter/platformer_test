#include "PCH.h"
#include "src/Tmx.h"
#include "Utilities.h"
#include "CoreUtils.h"

Level::Level():
	pMap(new Tmx::Map()),
	entities(),
	viDeleteIndex(),
	bLoaded(false),
	m_quad(Quadtree(0, sf::Rect<int>(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT)))
{
}


Level::~Level(void)
{
	UnInit();
}

// Un-initing the Level object. 
void Level::UnInit()
{
	// If our level is loaded, unload it first
	if (IsLevelLoaded())
		UnloadLevel();

	delete pMap;
}

// Unload a level and clear all data associated with the map
void Level::UnloadLevel()
{
	m_quad.clear();
	viDeleteIndex.clear();
	m_startPositions.clear();

	// Clear the grid
	for (std::vector<std::vector<Tile*>>::size_type i = 0; i < m_grid.size(); i++)
	{
		for (std::vector<Tile*>::size_type j = 0; j < m_grid[i].size(); j++)
		{
			delete m_grid[i][j];
		}

		m_grid[i].clear();
	}

	m_grid.clear();

	// Remove all entities from the level (and clean up their memory)
	for (std::vector<Entity*>::size_type i = 0; i < entities.size(); i++)
	{
		removeEntity(i);
	}

	entities.clear();

	bLoaded = false;
}

// Ends a level and goes back to the main menu
void Level::EndLevel()
{
	UnloadLevel();
	CORE.GetScreenManager()->GoToScreenType(SCREEN_TYPE_MAIN_MENU);
}

// Load a level and load the map data into a grid
bool Level::LoadLevel(const std::string& filepath)
{
	pMap->ParseFile(filepath);
	
	if (pMap->HasError())
	{
		UnInit();
		return false;
	}

	// Successful!
	bLoaded = true;

	// Init the map
	for (int i = 0; i < pMap->GetNumLayers(); ++i)
	{
		const Tmx::Layer* pLoopLayer = pMap->GetLayer(i);
		if (pLoopLayer->GetName() == "level")
		{
			for (int x = 0; x < pLoopLayer->GetWidth(); x++)
			{
				std::vector<Tile*> column;
				column.clear();

				for (int y = 0; y < pLoopLayer->GetHeight(); y++)
				{
					uint uTileID = pLoopLayer->GetTileId(x, y);
					
					sf::Texture texture;

					// Basic tile properties
					TileType eTileType = TILE_EMPTY;
					int iFloorLeft = 0, iFloorRight = 0;

					const Tmx::Tileset* pTileSet = pMap->FindTileset(uTileID);

					// Look for a tileset with the tile's gid and find it in the tileset info
					if (pTileSet)
					{
						const Tmx::Tile* pTile = pTileSet->GetTile(uTileID);
						if (pTile)
						{
							const Tmx::PropertySet propertySet = pTile->GetProperties();

							// Construct the texture

							std::string szImageSource = "../assets/maps/tilesets/" + pTileSet->GetImage()->GetSource();

							// Find out which part of the tileset image we have to crop out.
							int iTileID = pTile->GetId();
							int iRectLeft = (iTileID * TILE_SIZE) % pTileSet->GetImage()->GetWidth();
							int iRectTop = (iTileID * TILE_SIZE) / pTileSet->GetImage()->GetWidth();

							texture.loadFromFile(szImageSource, sf::IntRect(iRectLeft, iRectTop, TILE_SIZE, TILE_SIZE));

							std::string szProperty;
							szProperty = propertySet.GetStringProperty("TileType");
							if (!szProperty.empty())
							{
								if (szProperty == "TILE_SOLID")		eTileType = TILE_SOLID;
								if (szProperty == "TILE_ONE_WAY")	eTileType = TILE_ONE_WAY;
								if (szProperty == "TILE_LADDER")	eTileType = TILE_LADDER;
								if (szProperty == "TILE_SLOPE")		eTileType = TILE_SLOPE;
							}
							szProperty = propertySet.GetStringProperty("floor_left");
							if (!szProperty.empty())
							{
								iFloorLeft = atoi(szProperty.c_str());
							}
							szProperty = propertySet.GetStringProperty("floor_right");
							if (!szProperty.empty())
							{
								iFloorRight = atoi(szProperty.c_str());
							}
						}
					}

					Tile* pTempTile = new Tile(x, y, eTileType, texture, iFloorLeft, iFloorRight);
					column.push_back(pTempTile);
				}

				m_grid.push_back(column);
			}
		}
	}

	LoadPlayer();
	LoadObjects();

	return true;
}

void Level::LoadPlayer()
{
	LoadStartPositions();

	// Add the player
	addEntity(sf::Vector2f(16.0f, 32.0f), COMPONENT_PLAYER);

	GET_PLAYER()->GetWeaponController()->AddWeapon(CORE.GetWeaponData()[0], CORE.GetWeaponData()[0].maxammo, CORE.GetWeaponData()[0].magsize);
	GET_PLAYER()->GetWeaponController()->AddWeapon(CORE.GetWeaponData()[1], CORE.GetWeaponData()[1].maxammo, CORE.GetWeaponData()[1].magsize);
	GET_PLAYER()->GetWeaponController()->AddWeapon(CORE.GetWeaponData()[2], CORE.GetWeaponData()[2].maxammo, CORE.GetWeaponData()[2].magsize);
	moveToPosition(GET_PLAYER(), m_startPositions[0]);
}

void Level::LoadStartPositions()
{
	// Look through object groups
	for (int i = 0; i < pMap->GetNumObjectGroups(); i++)
	{
		const Tmx::ObjectGroup* pObjectGroup = pMap->GetObjectGroup(i);
		if (pObjectGroup)
		{
			if (pObjectGroup->GetName() == "start_positions")
			{
				const std::vector<Tmx::Object*> objects = pObjectGroup->GetObjects();

				// iterate through the objects and find the start_pos
				for (std::vector<Tmx::Object*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
				{
					if ((*it)->GetType() == "start_position")
					{
						// lol? these names...change them!
						StartPosition pos;
						pos.pos.x = (float)(*it)->GetX();
						pos.pos.y = (float)(*it)->GetY() + TILE_SIZE;

						m_startPositions.push_back(pos);
					}
				}
			}
		}
	}

	if (m_startPositions.empty())
	{
		StartPosition pos;
		pos.pos.x = 0.0f;
		pos.pos.y = 0.0f;
		m_startPositions.push_back(pos);
	}
}

// Private member function to load in map entities
void Level::LoadObjects()
{
	// Look through object groups
	for (int i = 0; i < pMap->GetNumObjectGroups(); i++)
	{
		const Tmx::ObjectGroup* pObjectGroup = pMap->GetObjectGroup(i);
		if (pObjectGroup)
		{
			if (pObjectGroup->GetName() == "entities")
			{
				const std::vector<Tmx::Object*> objects = pObjectGroup->GetObjects();

				// iterate through the objects
				for (std::vector<Tmx::Object*>::const_iterator it = objects.cbegin(); it != objects.cend(); ++it)
				{
					sf::Vector2f entitySize = sf::Vector2f((float)(*it)->GetWidth(), (float)(*it)->GetHeight());
					sf::Uint32 entityComponents = COMPONENT_NORMAL | COMPONENT_HEALTH;

					Entity* pNewEntity = addEntity(entitySize, entityComponents);

					pNewEntity->GetDisplacementController()->SetPositionX((float)(*it)->GetX());
					pNewEntity->GetDisplacementController()->SetPositionY((float)(*it)->GetY() + TILE_SIZE);

					pNewEntity->GetHealthController()->SetHealthTimes100(10000);

					if ((*it)->GetType() == "enemy")
						pNewEntity->SetIsHostile(true);
				}
			}
		}
	}
}

// Initialize the level and load the tiles.
void Level::Init()
{
}

void Level::processInput(sf::Event _event)
{
	if (!IsLevelLoaded()) return;

	Entity* pLoopEntity = NULL;
	// Check every entity for input
	for (std::vector<Entity>::size_type i = 0; i < entities.size(); i++)
	{
		pLoopEntity = entities[i];
		if (pLoopEntity != NULL)
		{
			if (pLoopEntity->GetInputController() != NULL)
			{
				pLoopEntity->GetInputController()->update(_event);
			}
		}
	}
}

// Update the level's entities
void Level::update()
{
	if (!IsLevelLoaded()) return;

	// Clear the deletion index
	viDeleteIndex.clear();

	// Update every entity
	for (std::vector<Entity>::size_type i = 0; i < entities.size(); i++)
	{
		Entity* pLoopEntity = entities[i];
		if (pLoopEntity != NULL)
		{
			// Entity is marked as dead...
			if (pLoopEntity->IsDelayedDeath())
			{
				// the player is dead...don't kill him!!!
				if (pLoopEntity == GET_PLAYER())
				{
					pLoopEntity->respawn();
				}
				// Another entity is dead - get the index to be deleted after processing.
				else
				{
					viDeleteIndex.push_back(i);
				}
			}
			else
			{
				pLoopEntity->update();
			}
		}
	}

	// COLLISION DETECTION
	// Clear the quadtree and start inserting ALIVE entities into it
	m_quad.clear();
	for (std::vector<Entity*>::size_type i = 0; i < entities.size(); i++)
	{
		if (entities[i]->IsDelayedDeath()) continue;
		m_quad.insert(entities[i]);
	}

	// Actually go through the quadtree now and handle collisions - O(n log n)
	std::vector<Entity*> returnObjects;
	for (std::vector<Entity*>::size_type i = 0; i < entities.size(); i++)
	{
		returnObjects.clear();

		m_quad.retrieve(returnObjects, entities[i]);

		for (std::vector<Entity*>::size_type x = 0; x < returnObjects.size(); x++)
		{
			if (entities[i] == entities[x]) continue;

			if (IsCollide(entities[i], entities[x]))
			{
				entities[i]->collide(entities[x]);
			}
		}
	}

	// DELETIONS - LAST THING TO BE UPDATED!
	// Go through the deletion index and delete those indices from entities()
	// this is an unordered operation and very efficient since we don't have to shift everything down
	for (std::vector<int>::size_type i = 0; i < viDeleteIndex.size(); i++)
	{
		int index = viDeleteIndex[i];
		// will permanently delete entity!
		removeEntity(index);
	}
}

// Draw the current level, based on the position of the player.
void Level::draw()
{
	if (!IsLevelLoaded()) return;

	float fCenterX, fCenterY;

	Entity* pPlayer = GET_PLAYER();

	fCenterX = pPlayer->GetDisplacementController()->GetPosition()->x;
	fCenterY = pPlayer->GetDisplacementController()->GetPosition()->y;

	// The number of tiles that will be drawn around the player. The player sits at the center of this, so it would be 25 in each direction.
	const sf::Uint8 TILES_ON_SCREEN = 50;

	int iStartX = int(fCenterX / TILE_SIZE) - TILES_ON_SCREEN/2;
	int iStartY = int(fCenterY / TILE_SIZE) - TILES_ON_SCREEN/2;
	int iEndX = iStartX + TILES_ON_SCREEN;
	int iEndY = iStartY + TILES_ON_SCREEN;

	// Draw the tiles
	for (int i = iStartX; i < iEndX; i++)
	{
		for (int j = iStartY; j < iEndY; j++)
		{
			Tile* pLoopTile = GetTile(i, j);
			if (pLoopTile)
				pLoopTile->draw();
		}
	}

	// Draw the entities
	for (std::vector<Entity>::size_type i = 0; i < GetEntities()->size(); i++)
	{
		Entity* pLoopEntity = GetEntity(i);
		if (pLoopEntity)
		{
			// Need to have a render controller in order to be drawn
			RenderController* pEntityRender = GetEntities()->at(i)->GetRenderController();
			if (pEntityRender != NULL)
				pEntityRender->draw();
		}
	}
}

// Move the player to a position
void Level::moveToPosition(Entity* pEntity, const Level::StartPosition& resetPos) const
{
	pEntity->GetDisplacementController()->SetPositionX(resetPos.pos.x);
	pEntity->GetDisplacementController()->SetPositionY(resetPos.pos.y);

	pEntity->GetDisplacementController()->SetDirty(true);
}

// Return the number of tiles of the level wide
int Level::GetTileWidth() const
{
	return pMap->GetWidth();
}

// Return the number of tiles of the level height
int Level::GetTileHeight() const
{
	return pMap->GetHeight();
}

// Returns the pixel coordinate of the bottom side of the level
float Level::GetHeight() const
{
	return (float) (pMap->GetHeight() * TILE_SIZE);
}

// Returns the pixel coordinate of the right side of the level
float Level::GetWidth() const
{
	return (float)(pMap->GetWidth() * TILE_SIZE);
}

// Return the tile at iX, iY
Tile* Level::GetTile(int iX, int iY) const
{
	if (!IsLevelLoaded()) return NULL;

	Tile* pTile = NULL;
	if (iX >= 0 && iX < (int)m_grid.size())
	{
		if (iY >= 0 && iY < (int)m_grid[iX].size())
		{
			pTile = m_grid[iX][iY];
		}
	}

	return pTile;
}

// Create a new entity and return a pointer to it
// Returns NULL if entity failed to create
Entity* Level::addEntity(sf::Vector2f size, sf::Uint32 components)
{
	if (!IsLevelLoaded()) return NULL;

	Entity* pNewEntity = new Entity(this, entities.size(), size, components);
	if (pNewEntity == NULL) return NULL;

	entities.push_back(pNewEntity);
	return pNewEntity;
}

// Remove the entity from the game (permanently!)
void Level::removeEntity(uint iIndex)
{
	if (!IsLevelLoaded()) return;

	// If the index is valid, then 
	if (iIndex < entities.size()-1)
	{
		// Get the entity
		Entity* pEntity = entities[iIndex];

		// Swap iIndex with the last entity
		entities[iIndex] = entities[entities.size() - 1];
		entities.pop_back();
		delete pEntity;
	}
	else if(iIndex == entities.size()-1)
	{
		entities.pop_back();
	}
}

std::vector<Entity*>* Level::GetEntities()
{
	return &entities;
}

Entity* Level::GetEntity(uint iIndex) const
{
	if (iIndex >= entities.size()) return NULL;
	return entities[iIndex];
}

//Helper function to get the player
Entity* Level::GetPlayer() const
{
	return entities[0];
}

std::vector<Level::StartPosition> Level::GetStartPositions() const
{
	return m_startPositions;
}

bool Level::IsLevelLoaded() const
{
	return bLoaded;
}