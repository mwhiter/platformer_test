 #include "PCH.h"

Tile::Tile(int x, int y, TileType eTileType, sf::Texture texture, int iFloorLeft, int iFloorRight) :
	x(x),
	y(y),
	iFloorLeft(iFloorLeft),
	iFloorRight(iFloorRight),
	texture(texture),
	eTileType(eTileType)
{
}

Tile::~Tile(void)
{
}

void Tile::draw()
{
	sf::Sprite sprite;
	sprite.setTexture(texture);
	sf::ConvexShape shape(4);
	sprite.setPosition((float)GetX() * TILE_SIZE, (float)GetY() * TILE_SIZE);
	core.GetRenderWindow()->draw(sprite);
}

int Tile::GetX()  const
{
	return x;
}

int Tile::GetY()  const
{
	return y;
}

bool Tile::IsSolid(Entity* pEntity, DirectionType eDirection)
{
	if(pEntity == NULL) return false;
	if(GetTileType() == TILE_EMPTY) return false;
	if (pEntity->GetDisplacementController() == NULL) return false;
	
	if (GetTileType() == TILE_SOLID)
	{
		return true;
	}

	switch(eDirection)
	{
		case DIRECTION_UP:
			break;
		case DIRECTION_DOWN:
			// Ladder tiles are only solid if the following conditions are met:
				// The ladder tile is the top-most tile in the ladder
				// The entity's bottom Y is fully above the topY of the tile
			if(GetTileType() == TILE_LADDER)
			{
				Tile* pAboveTile = pEntity->GetLevel()->GetTile(GetX(), GetY() - 1);
				if(pAboveTile != NULL)
				{
					if(pAboveTile->GetTileType() != TILE_LADDER && pEntity->GetBottomEdgeY() <= GetTopY())
						return true;
				}
			}
			// One-way tiles are only solid if the player is completely above it
			if(GetTileType() == TILE_ONE_WAY)
			{
				if(pEntity->GetBottomEdgeY() <= GetTopY())
				{
					return true;
				}
			}
			break;
		case DIRECTION_LEFT:
			// slope tiles are solid if the closest edge is the taller side and we're below it
			if (GetTileType() == TILE_SLOPE)
			{
				if (iFloorRight < iFloorLeft)
				{
					if (pEntity->GetBottomEdgeY() > GetTopY())
					{
						return true;
					}
				}
			}
			break;
		case DIRECTION_RIGHT:
			// slope tiles are solid if the closest edge is the taller side and we're below it
			if (GetTileType() == TILE_SLOPE)
			{
				if (iFloorLeft < iFloorRight)
				{
					if (pEntity->GetBottomEdgeY() > GetTopY())
					{
						return true;
					}
				}
			}
			break;
		default:
			break;
	}

	return false;
}

// Is this tile a slope?
bool Tile::IsSlope() const
{
	if(GetTileType() == TILE_SLOPE)
	{
		if(iFloorLeft != 0 || iFloorRight != 0)
		{
			return true;
		}
	}

	return false;
}

DirectionType Tile::GetSlopeDirection()
{
	DirectionType rtnValue = NO_DIRECTION_TYPE;
	if(IsSlope())
	{
		if(iFloorLeft < iFloorRight) return DIRECTION_LEFT;
		if(iFloorLeft > iFloorRight) return DIRECTION_RIGHT;
	}
	return rtnValue;
}

// Return the pixel location of tile's center X
float Tile::GetCenterX() const
{
	return GetLeftX() + float(TILE_SIZE / 2);
}

// Return the pixel location of tile's center Y
float Tile::GetCenterY() const
{
	return GetTopY() + float(TILE_SIZE / 2);
}

// Return pixel location of tile
float Tile::GetLeftX() const
{
	return (float) GetX() * TILE_SIZE;
}

// Return pixel location of tile
float Tile::GetRightX() const
{
	return (float) (GetX()+1) * TILE_SIZE;
}

// Return pixel location of tile
float Tile::GetBottomY() const
{
	return (float) (GetY()+1) * TILE_SIZE;
}

// Return pixel location of tile
float Tile::GetTopY() const
{
	return (float) (GetY()) * TILE_SIZE;
}

// Return the Y position of the left floor (relative to the top of tile)
float Tile::GetFloorLeftY() const
{
	return iFloorLeft + GetTopY();
}

// Return the Y position of the right floor (relative to the top of tile)
float Tile::GetFloorRightY() const
{
	return iFloorRight + GetTopY();
}


TileType Tile::GetTileType() const
{
	return eTileType;
}

void Tile::SetTileType(TileType eType)
{
	eTileType = eType;
}

void Tile::SetFloorLeft(int iNewValue)
{
	iNewValue = std::min(TILE_SIZE, iNewValue);

	iFloorLeft = iNewValue;
}
int Tile::GetFloorLeft() const
{
	return iFloorLeft;
}
void Tile::SetFloorRight(int iNewValue)
{
	iNewValue = std::min(TILE_SIZE, iNewValue);

	iFloorRight = iNewValue;
}
int Tile::GetFloorRight() const
{
	return iFloorRight;
}