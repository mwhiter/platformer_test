#include "PCH.h"
#include "CoreUtils.h"

DisplacementController::DisplacementController(Entity* pEntity):
	Component(pEntity),
	pGroundTile(NULL),
	pCeilingTile(NULL),
	pos(new sf::Vector2f(0.0f,0.0f)),
	dirty(true),
	veCurrentTiles()
{
}


DisplacementController::~DisplacementController(void)
{
}

sf::Vector2f* DisplacementController::GetPosition() const
{
	return pos;
}

void DisplacementController::SetPositionX(float fX)
{
	pos->x = fX;
}

void DisplacementController::SetPositionY(float fY)
{
	pos->y = fY;
}

void DisplacementController::ChangePositionX(float fChange)
{
	pos->x = pos->x + fChange;
}

void DisplacementController::ChangePositionY(float fChange)
{
	pos->y = pos->y + fChange;
}
// Update method on the displacement controller
void DisplacementController::update()
{
	Level* pLevel = pEntity->GetLevel();
	if (pLevel == NULL) return;

	pos->x = std::max(pEntity->GetSizeX() / 2, pos->x);
	pos->x = std::min(pos->x, pLevel->GetWidth() - pEntity->GetSizeX() / 2);

	// Correct movement in the Y direction
	pos->y = std::max(0.0f, pos->y);

	// Hit the bottom edge of the map - kill the player
	if (pEntity->GetBottomEdgeY() > pLevel->GetHeight())
	{
		pEntity->kill();
	}

	pos->y = std::min(pos->y, pLevel->GetHeight());

	// Check to see what our entity is doing
	CheckTouchingGround();
	CheckTouchingCeiling();

	Entity::WorldState eNewWorldState = Entity::NO_WORLD_STATE;
	pEntity->SetOnSlope(false);

	// Are we on a slope?
	if (GetSlopeTile() != NULL)
	{
		pEntity->SetOnSlope(true);
		eNewWorldState = Entity::WORLD_STATE_GROUND;
	}
	else
	{
		if (GetGroundTile() != NULL)
		{
			eNewWorldState = Entity::WORLD_STATE_GROUND;
		}
		else
		{
			eNewWorldState = Entity::WORLD_STATE_AIR;
		}
	}

	pEntity->SetWorldState(eNewWorldState);
}

// Calculate what tiles we're intersecting and store them in ppCurrentTiles
void DisplacementController::calcCurrentTiles()
{
	// Clear ppCurrentTiles
	for(std::vector<std::vector<Tile*>>::size_type i = 0; i < veCurrentTiles.size(); i++)
	{
		for(std::vector<Tile*>::size_type j = 0; j < veCurrentTiles[i].size(); j++)
		{
			veCurrentTiles[i][j] = NULL;
		}

		veCurrentTiles[i].clear();
	}
	veCurrentTiles.clear();

	Level* pLevel = pEntity->GetLevel();
	if(pLevel == NULL) return;

	float fEntityLeftX = pEntity->GetLeftEdgeX();
	float fEntityRightX = pEntity->GetRightEdgeX();
	float fEntityTopY = pEntity->GetTopEdgeY();
	float fEntityBottomY = pEntity->GetBottomEdgeY();

	int left	= int(fEntityLeftX / TILE_SIZE);
	int right	= int(fEntityRightX / TILE_SIZE);
	int top		= int(fEntityTopY / TILE_SIZE);
	int bottom	= int(fEntityBottomY / TILE_SIZE);

	if(int(fEntityBottomY) % 16 == 0) bottom--;
	if(int(fEntityRightX) % 16 == 0) right--;

	Tile* pLoopTile = NULL;
	for(int i = left; i <= right; i++)
	{
		std::vector<Tile*> tempCol;

		for(int j = top; j <= bottom; j++)
		{
			pLoopTile = pLevel->GetTile(i, j);
			if(pLoopTile != NULL)
			{
				tempCol.push_back(pLoopTile);
			}
		}

		// don't push back and empty column
		if (!tempCol.empty())
			veCurrentTiles.push_back(tempCol);
	}
	
	dirty = false;
}

// Do we intersect with a tile with tile-type?
Tile* DisplacementController::GetTileIntersectWithTileType(TileType eTileType)
{
	Level* pLevel = pEntity->GetLevel();
	if(pLevel == NULL) return NULL;

	std::vector<std::vector<Tile*>> viCurrentTiles = GetCurrentTiles();
	if(viCurrentTiles.empty()) return NULL;

	Tile* pLoopTile = NULL;
	for(std::vector<Tile*>::size_type i=0; i < viCurrentTiles.size(); i++)
	{
		for(std::vector<Tile*>::size_type j=0; j < viCurrentTiles[i].size(); j++)
		{
			int iX = viCurrentTiles[i][j]->GetX();
			int iY = viCurrentTiles[i][j]->GetY();
			pLoopTile = pLevel->GetTile(iX, iY);

			if(pLoopTile != NULL)
			{
				if(pLoopTile->GetTileType() == eTileType)
				{
					pLoopTile->test = true;
					return pLoopTile;
				}
			}
		}
	}

	return NULL;
}

// Are we standing on a slope tile?
Tile* DisplacementController::GetSlopeTile()
{
	Level* pLevel = pEntity->GetLevel();
	if(pLevel == NULL) return NULL;

	std::vector<std::vector<Tile*>> viCurrentTiles = GetCurrentTiles();

	Tile* pLoopTile = NULL;
	for(std::vector<Tile*>::size_type i=0; i < viCurrentTiles.size(); i++)
	{
		// ...only need to check the bottom row of tiles
		int iX = viCurrentTiles[i][viCurrentTiles[i].size()-1]->GetX();
		int iY = viCurrentTiles[i][viCurrentTiles[i].size()-1]->GetY();

		pLoopTile = pLevel->GetTile(iX, iY);
		if(pLoopTile->IsSlope())
		{
			return pLoopTile;
		}
	}

	return NULL;
}

std::vector<std::vector<Tile*>> DisplacementController::GetCurrentTiles()
{
	// dirty bit set to false
	if (dirty)
		calcCurrentTiles();

	return veCurrentTiles;
}

void DisplacementController::CheckTouchingGround()
{
	Level* pLevel = pEntity->GetLevel();
	if(pLevel == NULL) return;

	std::vector<std::vector<Tile*>> viCurrentTiles = GetCurrentTiles();
	if (viCurrentTiles.empty()) return;

	for(std::vector<Tile*>::size_type i=0; i < viCurrentTiles.size(); i++)
	{
		int iIndexX = viCurrentTiles[i][viCurrentTiles[i].size()-1]->GetX();
		int iIndexY = viCurrentTiles[i][viCurrentTiles[i].size()-1]->GetY()+1;

		Tile* pTileBelow = pLevel->GetTile(iIndexX, iIndexY);
		if(pTileBelow == NULL) continue;

		// Solid tile below us
		if(pTileBelow->IsSolid(pEntity, DIRECTION_DOWN))
		{
			// this is wonky...don't look at slope tiles
			if (pTileBelow->GetTileType() == TILE_SLOPE) continue;

			if (pEntity->GetBottomEdgeY() == pTileBelow->GetTopY())
			{
				SetGroundTile(pTileBelow);
				return;
			}
		}
	}

	// If we get down here then we're not touching solid ground
	SetGroundTile(NULL);
}

void DisplacementController::CheckTouchingCeiling()
{
	Level* pLevel = pEntity->GetLevel();
	if(pLevel == NULL) return;

	std::vector<std::vector<Tile*>> viCurrentTiles = GetCurrentTiles();
	if (viCurrentTiles.empty()) return;

	for(std::vector<Tile*>::size_type i=0; i < viCurrentTiles.size(); i++)
	{
		Tile* pLoopTile = viCurrentTiles[i][0];
		if (pLoopTile == NULL) continue;

		int iIndexX = viCurrentTiles[i][0]->GetX();
		int iIndexY = viCurrentTiles[i][0]->GetY()-1;

		Tile* pTileAbove = pLevel->GetTile(iIndexX, iIndexY);
		if(pTileAbove == NULL) continue;

		// Ceiling above us
		if(pTileAbove->IsSolid(pEntity, DIRECTION_UP))
		{
			// Actually below it
			if(pEntity->GetTopEdgeY() == pTileAbove->GetBottomY())
			{
				SetCeilingTile(pTileAbove);
				return;
			}
		}
	}

	// If we get down here then we're not touching the ceiling
	SetCeilingTile(NULL);
}

int DisplacementController::GetTopTileY() const
{
	if(veCurrentTiles.size() == 0) return -1;
	if(veCurrentTiles[0].size() == 0) return -1;

	return veCurrentTiles[0][0]->GetY();
}
int DisplacementController::GetBottomTileY() const
{
	if(veCurrentTiles.size() == 0) return -1;
	if(veCurrentTiles[0].size() == 0) return -1;

	return veCurrentTiles[0][veCurrentTiles[0].size()-1]->GetY();
}
int DisplacementController::GetLeftTileX() const
{
	if(veCurrentTiles.size() == 0) return -1;
	if(veCurrentTiles[0].size() == 0) return -1;

	return veCurrentTiles[0][0]->GetX();
}
int DisplacementController::GetRightTileX() const
{
	if(veCurrentTiles.size() == 0) return -1;
	if(veCurrentTiles[veCurrentTiles.size()-1].size() == 0) return -1;

	return veCurrentTiles[veCurrentTiles.size()-1][0]->GetX();
}

// Boilerplate stuff
Tile* DisplacementController::GetGroundTile() const
{
	return pGroundTile;
}
void DisplacementController::SetGroundTile(Tile* pTile)
{
	pGroundTile = pTile;
}
bool DisplacementController::IsOnSolidGround()
{
	return (GetGroundTile() != NULL);
}
Tile* DisplacementController::GetCeilingTile() const
{
	return pCeilingTile;
}
void DisplacementController::SetCeilingTile(Tile* pTile)
{
	pCeilingTile = pTile;
}
bool DisplacementController::IsTouchingCeiling() const
{
	return (GetCeilingTile() != NULL);
}
Entity* DisplacementController::GetEntity() const
{
	return pEntity;
}
void DisplacementController::SetDirty(bool bValue)
{
	if (dirty != bValue)
		dirty = bValue;
}