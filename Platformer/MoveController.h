#pragma once

#ifndef __MOVE_CONTROLLER_H
#define __MOVE_CONTROLLER_H

#include "PCH.h"
#include "Component.h"

// Forward declaration
class Entity;

class MoveController : public Component
{
public:
	enum MoveState
	{
		NO_MOVE_STATE = -1,

		MOVE_STATE_LEFT,
		MOVE_STATE_RIGHT,
		MOVE_STATE_LADDER,
		MOVE_STATE_LADDER_UP,
		MOVE_STATE_LADDER_DOWN,
		MOVE_STATE_LADDER_LEFT,
		MOVE_STATE_LADDER_RIGHT
	};

	MoveController(Entity* pEntity);
	~MoveController(void);

	sf::Vector2f* GetVelocity() const;
	sf::Vector2f* GetAcceleration() const;

	float GetSpeedX() const;
	void SetSpeedX(float fNewValue);
	float GetSpeedY() const;
	void SetSpeedY(float fNewValue);

	MoveState GetMoveState() const;
	void SetMoveState(MoveState eNewState);

	void move(DirectionType eDirection);
	void stop(DirectionType eDirection);

	void jump();
	void cancelJump();
	void crouch();
	void stand();

	bool grabLadder(DirectionType eDirection);
	void update();

	bool IsAffectedByGravity() const;	// should this entity be affected by gravity in its current state?
	bool IsGravityDisabled() const;		// is gravity disabled entirely for this entity?
	void SetGravityDisabled(bool bNewValue);

	Entity* GetEntity() const;
private:
	// Private methods
	void updateX();
	void updateY();

	void climbLadder(DirectionType eDirection);
	void endLadderState(DirectionType eDirection = NO_DIRECTION_TYPE);
	bool IsOnLadder() const;

	Tile* findBlockingTileX(float fVelX);
	Tile* findBlockingTileY(float fVelY, DirectionType eDirection);
	
	void setPositionSlope(Tile* slopeTile);

	void updateVelocityX();
	void updateVelocityY();

	MoveState eMoveState;

	sf::Vector2f* vel;
	sf::Vector2f* acc;

	float m_fVelJump;					// velocity from jumping

	float fSpeedX;
	float fSpeedY;

	bool m_bGravityDisabled;				// Is gravity disabled?
};

#endif