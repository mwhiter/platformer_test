#pragma once

#ifndef __TILE_H
#define __TILE_H

//////////////////////////////////////////////////////////////////
// Class: Tile
// - Stores information about a single tile in the game
// - Each tile has individual characteristics (like solid, ladder, etc.)
//////////////////////////////////////////////////////////////////

#include "enums.h"

class Tile
{
public:
	Tile(int x, int y, TileType eTileType, sf::Texture texture, int iFloorLeft = 0, int iFloorRight = 0);
	~Tile(void);

	int GetX() const;
	int GetY() const;

	float GetCenterX() const;
	float GetCenterY() const;
	float GetLeftX() const;
	float GetRightX() const;
	float GetBottomY() const;
	float GetTopY() const;
	float GetFloorLeftY() const;
	float GetFloorRightY() const;

	void SetFloorLeft(int iNewValue);
	int GetFloorLeft() const;
	void SetFloorRight(int iNewValue);
	int GetFloorRight() const;

	void draw();

	bool IsSolid(Entity* pEntity, DirectionType eDirection);
	bool IsSlope() const;
	DirectionType GetSlopeDirection();

	bool test;

	TileType GetTileType() const;
	void SetTileType(TileType eType);
private:
	TileType eTileType;
	int x, y;

	sf::Texture texture;

	// FLOOR LEFT AND FLOOR RIGHT
	// These two integer values refer to the amount of pixel offset from the top of the tile to the actual floor (e.g. on a slope)
	// A value of (4,7) would mean that the actual solid part starts on the 4th pixel down from the left side, and the 7th pixel down from the right side
	// The player will move an interpolated amount if he is anywhere between these values
	sf::Uint8 iFloorLeft, iFloorRight;
};

#endif