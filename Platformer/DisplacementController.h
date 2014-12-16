#pragma once

#ifndef __DISPLACEMENT_CONTROLLER_H
#define __DISPLACEMENT_CONTROLLER_H

#include "Component.h"

// Forward declaration
class Tile;

class DisplacementController : public Component
{
public:

	DisplacementController(Entity* pEntity);
	~DisplacementController(void);

	void update();

	// Current tiles the player occupies
	void calcCurrentTiles();
	std::vector<std::vector<Tile*>> GetCurrentTiles();

	Tile* GetGroundTile() const;
	void SetGroundTile(Tile* pTile);
	bool IsOnSolidGround();

	Tile* GetCeilingTile() const;
	void SetCeilingTile(Tile* pTile);
	bool IsTouchingCeiling() const;

	Tile* GetTileIntersectWithTileType(TileType eTileType);
	Tile* GetTileIntersectWithTileType(TileType eTileType, float fX);
	Tile* GetSlopeTile();

	int GetTopTileY() const;
	int GetBottomTileY() const;
	int GetLeftTileX() const;
	int GetRightTileX() const;

	sf::Vector2f* GetPosition() const;
	void SetPositionX(float fX);
	void ChangePositionX(float fChange);
	void SetPositionY(float fY);
	void ChangePositionY(float fChange);

	void SetDirty(bool bValue);

	Entity* GetEntity() const;
private:
	void CheckTouchingGround();
	void CheckTouchingCeiling();

	sf::Vector2f* pos;

	std::vector<std::vector<Tile*>> veCurrentTiles;

	Tile* pGroundTile;
	Tile* pCeilingTile;

	bool dirty;
};

#endif