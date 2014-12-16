#include "PCH.h"
#include "CoreUtils.h"

MoveController::MoveController(Entity* pEntity):
	Component(pEntity),
	eMoveState(NO_MOVE_STATE),
	vel(new sf::Vector2f(0,0)),
	acc(new sf::Vector2f(0,0)),
	fSpeedX(0.0f),
	fSpeedY(0.0f),
	m_fVelJump(0.0f),
	m_bGravityDisabled(false)
{
}

MoveController::~MoveController(void)
{
}

float MoveController::GetSpeedX() const
{
	return fSpeedX;
}

void MoveController::SetSpeedX(float fNewValue)
{
	fSpeedX = fNewValue;
}

float MoveController::GetSpeedY() const
{
	return fSpeedY;
}

void MoveController::SetSpeedY(float fNewValue)
{
	fSpeedY = fNewValue;
}

sf::Vector2f* MoveController::GetVelocity() const
{
	return vel;
}

sf::Vector2f* MoveController::GetAcceleration() const
{
	return acc;
}

MoveController::MoveState MoveController::GetMoveState() const
{
	return eMoveState;
}

void MoveController::SetMoveState(MoveController::MoveState eNewState)
{
	eMoveState = eNewState;
}

// Move toward eDirection
void MoveController::move(DirectionType eDirection)
{
	// A few notes with this function -
		// it does seem to make sense to separate our movement out when we're on a ladder vs. when we're not
		// but it doesn't seem to make sense to separate our facing when we're on a ladder vs. when we're not
		// Solution? Separate facing and movement. This is a pretty limited implementation. Improve!

	if(IsOnLadder())
	{
		switch(eDirection)
		{
		case DIRECTION_UP:
			SetMoveState(MOVE_STATE_LADDER_UP);
			break;
		case DIRECTION_DOWN:
			SetMoveState(MOVE_STATE_LADDER_DOWN);
			break;
		case DIRECTION_LEFT:
			SetMoveState(MOVE_STATE_LADDER_LEFT);
			pEntity->SetFacing(DirectionType(pEntity->GetFacing() | DIRECTION_LEFT));
			break;
		case DIRECTION_RIGHT:
			SetMoveState(MOVE_STATE_LADDER_RIGHT);
			pEntity->SetFacing(DirectionType(pEntity->GetFacing() | DIRECTION_RIGHT));
			break;
		}
	}
	else
	{
		DirectionType eNewFacing = NO_DIRECTION_TYPE;
		
		switch(eDirection)
		{
		case DIRECTION_UP:
			if (!grabLadder(eDirection))
			{
				eNewFacing = DirectionType(pEntity->GetFacing() | DIRECTION_UP);
				
				// we may be facing down when we switch to Up, so negate it if we are
				if ((eNewFacing & DIRECTION_DOWN) == DIRECTION_DOWN)
					eNewFacing = DirectionType(eNewFacing ^ DIRECTION_DOWN);

				pEntity->SetFacing(eNewFacing);
			}
			break;
		case DIRECTION_DOWN:
			// If we didn't grab the ladder, then set our facing up
			if (!grabLadder(eDirection))
			{
				eNewFacing = DirectionType(pEntity->GetFacing() | DIRECTION_DOWN);
				
				// we may be facing up when we switch to down, so negate it if we are
				if ((eNewFacing & DIRECTION_UP) == DIRECTION_UP)
					eNewFacing = DirectionType(eNewFacing ^ DIRECTION_UP);
				
				pEntity->SetFacing(eNewFacing);
			}
			break;
		case DIRECTION_LEFT:
			SetMoveState(MOVE_STATE_LEFT);
			eNewFacing = DirectionType(pEntity->GetFacing() | DIRECTION_LEFT);
			
			// we may be facing right when we switch to left, so negate it if we are
			if ((eNewFacing & DIRECTION_RIGHT) == DIRECTION_RIGHT)
				eNewFacing = DirectionType(eNewFacing ^ DIRECTION_RIGHT);
			
			pEntity->SetFacing(eNewFacing);
			break;
		case DIRECTION_RIGHT:
			SetMoveState(MOVE_STATE_RIGHT);
			eNewFacing = DirectionType(pEntity->GetFacing() | DIRECTION_RIGHT);
			
			// we may be facing right when we switch to left, so negate it if we are
			if ((eNewFacing & DIRECTION_LEFT) == DIRECTION_LEFT)
				eNewFacing = DirectionType(eNewFacing ^ DIRECTION_LEFT);
			
			pEntity->SetFacing(eNewFacing);
			break;
		}
	}
}

// Stop in eDirection
void MoveController::stop(DirectionType eDirection)
{
	switch(eDirection)
	{
	case DIRECTION_UP:
		if(IsOnLadder())
		{
			SetMoveState(MOVE_STATE_LADDER);
		}
		pEntity->SetFacing(DirectionType(pEntity->GetFacing() ^ DIRECTION_UP));
		break;
	case DIRECTION_DOWN:
		if(IsOnLadder())
		{
			SetMoveState(MOVE_STATE_LADDER);
		}
		pEntity->SetFacing(DirectionType(pEntity->GetFacing() ^ DIRECTION_DOWN));
		break;
	case DIRECTION_LEFT:
		if(eMoveState == MOVE_STATE_LEFT)
		{
			SetMoveState(NO_MOVE_STATE);
		}
		break;
	case DIRECTION_RIGHT:
		if(eMoveState == MOVE_STATE_RIGHT)
		{
			SetMoveState(NO_MOVE_STATE);
		}
		break;
	}
}

// pEntity jumps
void MoveController::jump()
{
	if(pEntity == NULL) return;
	if(IsOnLadder())
	{
		endLadderState();
		return;
	}
	
	// Can't be touching the ceiling
	if(pEntity->GetDisplacementController()->IsTouchingCeiling())
		return;
	
	// Can't be in the air
	if (pEntity->GetWorldState() == Entity::WORLD_STATE_AIR)
	{
		return;
	}

	vel->y = VELOCITY_JUMP;
	pEntity->SetWorldState(Entity::WORLD_STATE_AIR);
}

// pEntity grabs onto a ladder at pTile
bool MoveController::grabLadder(DirectionType eDirection)
{
	if(pEntity == NULL) return false;
	if(eDirection == MOVE_STATE_LADDER_LEFT || eDirection == MOVE_STATE_LADDER_RIGHT) return false;

	Tile* pTile = NULL;

	pTile = pEntity->GetDisplacementController()->GetTileIntersectWithTileType(TILE_LADDER);


	bool bNeedToAdjustHeight = false;

	// pTile was null but we are heading down. It's possible that we're on top of a ladder tile and want to go down. So, check it
	if(eDirection == DIRECTION_DOWN && pTile == NULL)
	{
		Level* pLevel = pEntity->GetLevel();
		if(pLevel != NULL)
		{
			std::vector<std::vector<Tile*>> viCurrentTiles = pEntity->GetDisplacementController()->GetCurrentTiles();

			Tile* pLoopTile = NULL;
			for(std::vector<Tile*>::size_type i=0; i < viCurrentTiles.size(); i++)
			{
				int iX = viCurrentTiles[i][viCurrentTiles[i].size()-1]->GetX();
				int iY = viCurrentTiles[i][viCurrentTiles[i].size()-1]->GetY() + 1;	// will search tiles the row below the entity
				pLoopTile = pLevel->GetTile(iX, iY);

				if(pLoopTile != NULL)
				{
					if(pLoopTile->GetTileType() == TILE_LADDER)
					{
						pTile = pLoopTile;
						bNeedToAdjustHeight = true;
					}
				}
			}
		}
	}

	if (pTile == NULL) return false;

	// Set the entity state to ladder and position player to that tile
	SetMoveState(MOVE_STATE_LADDER);
	vel->x = 0;
	vel->y = 0;
	pEntity->GetDisplacementController()->SetPositionX(pTile->GetLeftX() + TILE_SIZE/2);
	if(bNeedToAdjustHeight)
		pEntity->GetDisplacementController()->SetPositionY(pTile->GetTopY());

	return true;
}

// pEntity climbs ladder toward direction
// Much more simplified version of normal movement, because we don't concern ourselves with blocking tiles or not (at least as much)
void MoveController::climbLadder(DirectionType eDirection)
{	
	if(pEntity == NULL) return;
	if(pEntity->GetDisplacementController() == NULL) return;
	Level* pLevel = pEntity->GetLevel();

	float fMoveVel = 0.0f;
	Tile* pBlockingTile = NULL;
	bool bEndLadderState = false;

	switch(eDirection)
	{
	case DIRECTION_UP:
		fMoveVel = -CLIMB_SPEED;
		pBlockingTile = findBlockingTileY(fMoveVel, DIRECTION_UP);
		break;
	case DIRECTION_DOWN:
		fMoveVel = CLIMB_SPEED;
		pBlockingTile = findBlockingTileY(fMoveVel, DIRECTION_DOWN);
		break;
	case DIRECTION_LEFT:
	case DIRECTION_RIGHT:
	default:
		bEndLadderState = true;
		break;
	}
	
	// Check to see if we got blocked by a tile - floor or ceiling
	if (!bEndLadderState)
	{
		float distFromObstacle = 0.0f;
		// Tile blocked us
		if (pBlockingTile != NULL)
		{
			// if it's our floor tile
			if (eDirection == DIRECTION_DOWN)
			{
				distFromObstacle = pBlockingTile->GetTopY() - pEntity->GetBottomEdgeY();
				if (distFromObstacle < fMoveVel)
				{
					fMoveVel = distFromObstacle;
					bEndLadderState = true;
				}
			}
			// if it's a ceiling tile, then do nothing but don't move into it...
			else if (eDirection == DIRECTION_UP)
			{
				distFromObstacle = pBlockingTile->GetBottomY() - pEntity->GetTopEdgeY();
				if (abs(distFromObstacle) < abs(fMoveVel))
				{
					fMoveVel = distFromObstacle;
				}
			}
		}

		pEntity->GetDisplacementController()->ChangePositionY(fMoveVel);
		pEntity->GetDisplacementController()->SetDirty(true);
	}

	// Check to see if we reached the top of a ladder
	if (eDirection == DIRECTION_UP)
	{
		if (!bEndLadderState)
		{
			// Will return the ladder tile that we intersect at the top
			Tile* pLadderTile = pEntity->GetDisplacementController()->GetTileIntersectWithTileType(TILE_LADDER);
			if (pLadderTile)
			{
				Tile* pTileAboveLadder = pEntity->GetLevel()->GetTile(pLadderTile->GetX(), pLadderTile->GetY() - 1);
				if (pTileAboveLadder)
				{
					// Didn't find a ladder tile above this
					if (pTileAboveLadder->GetTileType() != TILE_LADDER)
					{
						// The middle of our entity is above the topY of the ladder tile we're on, then we've reached the top of the ladder
						if (pEntity->GetDisplacementController()->GetPosition()->y < pLadderTile->GetTopY())
						{
							// Move us on top of the ladder (to-do, play animation?)
							pEntity->GetDisplacementController()->SetPositionY(pLadderTile->GetTopY() - pEntity->GetSizeY()/2);
							bEndLadderState = true;
						}
					}
				}
			}
			// for some reason we're not intersecting a ladder tile - just end the ladder state now then
			else
			{
				bEndLadderState = true;
			}
		}
	}

	// Check to see if we reached the bottom of a ladder. Much simplier since we just see if we intersect with a ladder anymore
	if (eDirection == DIRECTION_DOWN)
	{
		if (!bEndLadderState)
		{
			// Will return the ladder tile that we intersect at the top
			Tile* pLadderTile = pEntity->GetDisplacementController()->GetTileIntersectWithTileType(TILE_LADDER);
			if (!pLadderTile)
			{
				bEndLadderState = true;
			}
		}
	}

	if (bEndLadderState)
		endLadderState(eDirection);
}

// End the ladder state - optional direction state
void MoveController::endLadderState(DirectionType eDirection)
{
	if(pEntity == NULL) return;
	if(pEntity->GetDisplacementController() == NULL) return;

	SetMoveState(NO_MOVE_STATE);

	// Move the player in a certain direction when we get out of a ladder
	if(eDirection == DIRECTION_LEFT)
		SetMoveState(MOVE_STATE_LEFT);
	else if(eDirection == DIRECTION_RIGHT)
		SetMoveState(MOVE_STATE_RIGHT);
	
	vel->y = 0.0f;
	
	// is there a better way to do this?
	pEntity->GetDisplacementController()->SetDirty(true);
}

// The entity crouches
void MoveController::crouch()
{
	if (pEntity->GetDisplacementController() == NULL)
		return;

	// No crouching in the air!
	if (pEntity->GetWorldState() == Entity::WORLD_STATE_AIR)
		return;

	// Can't crouch if we already are!
	if (pEntity->GetState() == Entity::STATE_CROUCH)
		return;

	float old_y = pEntity->GetDisplacementController()->GetPosition()->y;
	
	// Hardcoding this now. Want this to be entity-specific
	float fCrouchHeight = (float) TILE_SIZE;

	pEntity->SetSize(sf::Vector2f(pEntity->GetSizeX(), fCrouchHeight));
	pEntity->SetState(Entity::STATE_CROUCH);
	pEntity->GetDisplacementController()->SetPositionY(old_y + pEntity->GetSizeY()/2);
}

// The entity stands up after crouching
void MoveController::stand()
{
	// Can't stand up if we already are!
	if (pEntity->GetState() == Entity::STATE_STANDING)
		return;

	float old_y = pEntity->GetDisplacementController()->GetPosition()->y;
	
	// Hardcoding this now. Want this to be entity-specific
	float fStandHeight = (float)TILE_SIZE * 2;

	// Double the entities' size
	// Wrong - what I want to do is read this in as data parameter
	pEntity->SetSize(sf::Vector2f(pEntity->GetSizeX(), fStandHeight));
	pEntity->SetState(Entity::STATE_STANDING);
	pEntity->GetDisplacementController()->SetPositionY(old_y - pEntity->GetSizeY() / 2);
}

// Are we on a ladder?
bool MoveController::IsOnLadder() const
{
	if(pEntity == NULL) return false;

	switch(eMoveState)
	{
	case MOVE_STATE_LADDER:
	case MOVE_STATE_LADDER_UP:
	case MOVE_STATE_LADDER_DOWN:
	case MOVE_STATE_LADDER_RIGHT:
	case MOVE_STATE_LADDER_LEFT:
		return true;
	}

	return false;
}

// Update the movement of pEntity. Step X first, then Y
void MoveController::update()
{
	if(pEntity == NULL) return;
	if(pEntity->GetDisplacementController() == NULL) return;
	
	if(IsOnLadder())
	{
		switch(eMoveState)
		{
		case MOVE_STATE_LADDER_UP:
			climbLadder(DIRECTION_UP);
			break;
		case MOVE_STATE_LADDER_DOWN:
			climbLadder(DIRECTION_DOWN);
			break;
		case MOVE_STATE_LADDER_LEFT:
			climbLadder(DIRECTION_LEFT);
			return;
			break;
		case MOVE_STATE_LADDER_RIGHT:
			climbLadder(DIRECTION_RIGHT);
			break;
		default:
			break;
		}
	}
	else
	{
		updateX();
		updateY();
	}
}

//------------------------------------------------------------
// MOVEMENT - X
// Calculate velocity in x direction
void MoveController::updateVelocityX()
{
	float newVelocity = vel->x;

	// Which direction are we moving? Assign acceleration in the direction we are moving
	switch (GetMoveState())
	{
	case MOVE_STATE_LEFT:
		if (acc->x > 0.0f)
		{
			acc->x = 0.0f;
			newVelocity = 0.0f;
		}
		GetAcceleration()->x = -ACCEL_INPUT;
		break;
	case MOVE_STATE_RIGHT:
		if (GetAcceleration()->x < 0.0f)
		{
			acc->x = 0.0f;
			newVelocity = 0.0f;
		}
		GetAcceleration()->x = ACCEL_INPUT;
		break;
	case NO_MOVE_STATE:
		acc->x = 0.0f;
		newVelocity = 0.0f;
		break;
	default:
		return;
	}

	newVelocity += GetAcceleration()->x;

	// Can't go faster than Max X Velocity
	newVelocity = std::min(newVelocity, VELOCITY_MAX_X);
	newVelocity = std::max(newVelocity, -VELOCITY_MAX_X);

	vel->x = newVelocity;
}

// Step movement in X direction
void MoveController::updateX()
{
	if(eMoveState == MOVE_STATE_LADDER) return;	// in the ladder state, movement can only be done on the Y direction

	sf::Vector2f* pPosition = pEntity->GetDisplacementController()->GetPosition();
	Level* pLevel = pEntity->GetLevel();
	
	updateVelocityX();

	DirectionType eMoveDirection = eMoveState == MOVE_STATE_LEFT ? DIRECTION_LEFT : DIRECTION_RIGHT;

	// Get our velocity
	float fVelX = abs(vel->x);
	if(fVelX == 0.0f) return;

	// Get the forward edge of our bounding box
	// This should really be a function of Entity
	float fForwardEdgeX = 0.0f;
	if(eMoveState == MOVE_STATE_LEFT) fForwardEdgeX = pEntity->GetLeftEdgeX();
	if(eMoveState == MOVE_STATE_RIGHT) fForwardEdgeX = pEntity->GetRightEdgeX();

	// Find a tile that's blocking our position
	Tile* pBlockingTile = findBlockingTileX(fVelX);
	float distFromObstacle = 0.0f;
 	float pObstacleX = 0.0f;

	// There's a tile blocking us, so adjust velocity so we don't pass through it
	if(pBlockingTile != NULL)
	{
		if(eMoveState == MOVE_STATE_LEFT)
		{
			pObstacleX = pBlockingTile->GetRightX();
			distFromObstacle = fForwardEdgeX - pObstacleX;
		}
		if(eMoveState == MOVE_STATE_RIGHT)
		{
			pObstacleX = pBlockingTile->GetLeftX();
			distFromObstacle = pObstacleX - fForwardEdgeX;
		}
		
		// Blocked by a tile
		if(abs(distFromObstacle) < fVelX)
		{	
			fVelX = abs(distFromObstacle);
		}
	}

	// If we're moving left, reverse the velocity
	fVelX = eMoveState == MOVE_STATE_LEFT ? -fVelX : fVelX;
	pPosition->x += fVelX;

	pEntity->GetDisplacementController()->SetDirty(true);
}

// Find a tile blocking our movement in the X direction
Tile* MoveController::findBlockingTileX(float fVelX)
{
	sf::Vector2f* pPosition = pEntity->GetDisplacementController()->GetPosition();
	Level* pLevel = pEntity->GetLevel();

	// Direction of movement
	DirectionType eDirection = eMoveState == MOVE_STATE_LEFT ? DIRECTION_LEFT : DIRECTION_RIGHT;

	// Get our current tiles
	std::vector<std::vector<Tile*>> veCurrentTiles = pEntity->GetDisplacementController()->GetCurrentTiles();
	if(veCurrentTiles.empty()) return NULL;

	int iStartIndexX = -1;
	int iEndIndexX = -1;

	Tile* pLeftTile = veCurrentTiles[0][0];
	if(pLeftTile == NULL) return NULL;
	Tile* pRightTile = veCurrentTiles[veCurrentTiles.size()-1][0];
	if(pRightTile == NULL) return NULL;

	// Which side of the entity are we checking?
	if(eMoveState == MOVE_STATE_LEFT) iStartIndexX = pLeftTile->GetX();						// Left side
	if(eMoveState == MOVE_STATE_RIGHT) iStartIndexX = pRightTile->GetX();					// Right side
	
	// Add to the end index any tiles we MAY hit this frame (via our velocity)
	int iTilesWeCouldPassX = std::max(int(fVelX / TILE_SIZE + 1), 1);
	
	if(eMoveState == MOVE_STATE_LEFT) iEndIndexX = std::max(0, iStartIndexX - iTilesWeCouldPassX);
	if(eMoveState == MOVE_STATE_RIGHT) iEndIndexX = std::min(pLevel->GetTileWidth(), iStartIndexX + iTilesWeCouldPassX + 1);

	int iStartIndexY = -1;	
	int iEndIndexY = -1;

	Tile* pBottomTile = veCurrentTiles[0][veCurrentTiles[0].size()-1];
	if(pBottomTile == NULL) return NULL;

	iStartIndexY = pLeftTile->GetY();
	iEndIndexY = pBottomTile->GetY();

	Tile* pRtnTile = NULL;

	// Look through tiles in front of us that might be solid...
	// There's a lot of 'clever' code here but it's to make everything short and compact
	// The outer for loop condition controls the direction of the loop.
		//If we're moving left, then we want to move left, hence 'i' is decremented and we check for >= iEndIndex
		//If we're moving right, then we want to move right, hence 'i' is incremented and we check for i < iEndIndex

	for(int i = iStartIndexX;
		(eMoveState == MOVE_STATE_LEFT ? i >= iEndIndexX : i < iEndIndexX);
		(eMoveState == MOVE_STATE_LEFT ? i--: i++))
	{
		for(int j = iStartIndexY; j <= iEndIndexY; j++)
		{
			pRtnTile = pLevel->GetTile(i, j);
			if(pRtnTile == NULL) continue;

			// If we're standing on a slope...
			Tile* pSlopeTile = pEntity->GetDisplacementController()->GetSlopeTile();
			if (pSlopeTile)
			{
				// and we're moving towards the direction of the slope...
				if (pSlopeTile->GetSlopeDirection() == eDirection)
				{
					// don't bother processing
					continue;
				}
			}

			// Found a solid tile in our way
			if(pRtnTile->IsSolid(pEntity, eDirection))
			{
 				return pRtnTile;
			}
		}
	}

	return NULL;
}

//------------------------------------------------------------
// MOVEMENT - Y
// Calculate velocity in y direction
void MoveController::updateVelocityY()
{
	float newVelocity = vel->y;

	// Apply gravity
	if(IsAffectedByGravity())
	{
 		GetAcceleration()->y = ACCEL_GRAVITY;
	}

	newVelocity += GetAcceleration()->y;

	// If we're on the ground, then cancel out positive velocity (downwards)
	if (pEntity->GetDisplacementController()->IsOnSolidGround())	newVelocity = std::min(0.0f, newVelocity);

	// If we're touching the ceiling, then cancel out negative velocity (upwards)
	if (pEntity->GetDisplacementController()->IsTouchingCeiling())	newVelocity = std::max(0.0f, newVelocity);

	// Little hack to prevent us from gaining too much velocity on slopes...
	if (pEntity->IsOnSlope()) newVelocity = std::min(4.0f, newVelocity);

	// Can't fall faster than terminal velocity
	newVelocity = std::min(newVelocity, TERMINAL_VELOCITY);

	vel->y = newVelocity;
}

// Step movement in Y direction
void MoveController::updateY()
{
	updateVelocityY();	

	Tile* pSlopeTile = pEntity->GetDisplacementController()->GetTileIntersectWithTileType(TILE_SLOPE);
	if (pSlopeTile != NULL)
	{
		setPositionSlope(pSlopeTile);
		return;
	}

	// Which direction are we moving in?
	DirectionType eMoveDirection = NO_DIRECTION_TYPE;
	if(vel->y < 0)
	{
		eMoveDirection = DIRECTION_UP;
	}
	else if (vel->y > 0)
	{
		eMoveDirection = DIRECTION_DOWN;
	}
	if(eMoveDirection == NO_DIRECTION_TYPE) return;

	sf::Vector2f* pPosition = pEntity->GetDisplacementController()->GetPosition();
	Level* pLevel = pEntity->GetLevel();

	float fVelY = abs(vel->y);

	// Get the forward edge of our bounding box
	float fForwardEdgeY = 0.0f;
	if(eMoveDirection == DIRECTION_DOWN) fForwardEdgeY = pEntity->GetBottomEdgeY();
	if(eMoveDirection == DIRECTION_UP) fForwardEdgeY = pEntity->GetTopEdgeY();

	// Find a tile that's blocking our position
	Tile* pBlockingTile = findBlockingTileY(fVelY, eMoveDirection);

	// There's a tile blocking us, so adjust velocity so we don't pass through it
	if(pBlockingTile != NULL)
	{
		float pObstacleY = 0.0f;
		float distFromObstacle = 0.0f;

		if(eMoveDirection == DIRECTION_DOWN)
		{
			pObstacleY = pBlockingTile->GetTopY();
 			distFromObstacle = pObstacleY - fForwardEdgeY;
		}
		else
		{
			pObstacleY = pBlockingTile->GetBottomY();
			distFromObstacle = fForwardEdgeY - pObstacleY;
		}
		
		// There's a tile blocking us and it's close enough that we hit it
		if(distFromObstacle < fVelY)
		{
			fVelY = distFromObstacle;
		}
	}

	fVelY = (eMoveDirection == DIRECTION_UP) ? -fVelY : fVelY;

	pPosition->y += fVelY;

	pEntity->GetDisplacementController()->SetDirty(true);
}

// Find a tile blocking our movement in the Y direction
Tile* MoveController::findBlockingTileY(float fVelY, DirectionType eDirection)
{
	sf::Vector2f* pPosition = pEntity->GetDisplacementController()->GetPosition();
	
	Level* pLevel = pEntity->GetLevel();

	// Get our current tiles
	std::vector<std::vector<Tile*>> veCurrentTiles = pEntity->GetDisplacementController()->GetCurrentTiles();
	if(veCurrentTiles.empty()) return NULL;

	int iStartIndexY = -1;
	int iEndIndexY = -1;

	// Which side of the entity are we checking?
	Tile* pTopTile = veCurrentTiles[0][0];
	if(pTopTile == NULL) return NULL;

	Tile* pBottomTile = veCurrentTiles[0][veCurrentTiles[0].size()-1];
	if(pBottomTile == NULL) return NULL;

	if (eDirection == DIRECTION_DOWN) iStartIndexY = pBottomTile->GetY();	// Bottom side
	if(eDirection == DIRECTION_UP) iStartIndexY = pTopTile->GetY();	// Top side

	// Add to the end index any tiles we MAY hit this frame (via our velocity)
	int iTilesWeCouldPass = std::max(int(fVelY / TILE_SIZE + 1), 1);

	if(eDirection == DIRECTION_DOWN) iEndIndexY = std::min(pLevel->GetTileHeight(), iStartIndexY + iTilesWeCouldPass + 1);
	if(eDirection == DIRECTION_UP) iEndIndexY = std::max(0, iStartIndexY - iTilesWeCouldPass);
	
	int iStartIndexX = -1;
	int iEndIndexX = -1;

	Tile* pRightTile = veCurrentTiles[veCurrentTiles.size()-1][0];
	if(pRightTile == NULL) return NULL;

	iStartIndexX = pTopTile->GetX();
	iEndIndexX = pRightTile->GetX();

	Tile* pRtnTile = NULL;

	// Look through tiles in front of us that might be solid...
	// There's a lot of 'clever' code here but it's to make everything short and compact
	// The outer for loop condition controls the direction of the loop.
		//If we're moving left, then we want to move left, hence 'i' is decremented and we check for >= iEndIndex
		//If we're moving right, then we want to move right, hence 'i' is incremented and we check for i < iEndIndex

	float fClosestEdgeY = eDirection == DIRECTION_DOWN ? FLT_MAX : -FLT_MAX;
	Tile* pLoopTile = NULL;

	for(int j = iStartIndexX; j <= iEndIndexX; j++)
	{
		for(int i = iStartIndexY;
			(eDirection == DIRECTION_DOWN ? i < iEndIndexY : i >= iEndIndexY);
			(eDirection == DIRECTION_DOWN ? i++ : i--))
		{
			pLoopTile = pLevel->GetTile(j, i);
			if (pLoopTile == NULL) continue;

			// Found a solid tile in our way
			if (pLoopTile->IsSolid(pEntity, eDirection))
			{
				// Only care about the edge if it's the closest
				if (eDirection == DIRECTION_DOWN ? pLoopTile->GetTopY() < fClosestEdgeY : pLoopTile->GetBottomY() > fClosestEdgeY)
				{
					fClosestEdgeY = (eDirection == DIRECTION_DOWN ? pLoopTile->GetTopY() : pLoopTile->GetBottomY());
					pRtnTile = pLoopTile;
				}
			}
		}
	}

	// can be null (didn't find an edge)
	return pRtnTile;
}

// Correct position when we're standing on a slope
void MoveController::setPositionSlope(Tile* pSlopeTile)
{
	if (pSlopeTile == NULL) return;
	if (pEntity->GetDisplacementController() == NULL) return;

	sf::Vector2f* pPosition = pEntity->GetDisplacementController()->GetPosition();
	DirectionType eSlopeDirection = pSlopeTile->GetSlopeDirection();

	// Store our velocities into variables
	float fVelocityY = vel->y;

	float fEntityMiddleX = pPosition->x;
	float fEntityBottomY = pEntity->GetBottomEdgeY();

	float fSlopeTileLeftX = pSlopeTile->GetLeftX();

	// How far into the tile are we?
	float fPercentX = (fEntityMiddleX - fSlopeTileLeftX) / (float)TILE_SIZE;

	// Check fPercentX if we're the last tile in a slope - fixes a bug
	// Is there a better way to do this?
	if (fPercentX > 1.0f)
	{
		Tile* pOtherTile;

		// Don't have this happen if we're on the middle of a slope
		pOtherTile = pEntity->GetLevel()->GetTile(pSlopeTile->GetX() + 1, pSlopeTile->GetY());
		if (pOtherTile)
		{
			if (!pOtherTile->IsSlope())
			{
				pOtherTile = pEntity->GetLevel()->GetTile(pSlopeTile->GetX() + 1, pSlopeTile->GetY() + 1);
				if (pOtherTile)
				{
					if (!pOtherTile->IsSlope())
					{
						fPercentX = 1.0f;
					}
				}
			}
		}
	}
	// in the other direction as well!
	else if (fPercentX < 0.0f)
	{
		Tile* pOtherTile;

		// Don't have this happen if we're on the middle of a slope
		pOtherTile = pEntity->GetLevel()->GetTile(pSlopeTile->GetX() - 1, pSlopeTile->GetY());
		if (pOtherTile)
		{
			if (!pOtherTile->IsSlope())
			{
				pOtherTile = pEntity->GetLevel()->GetTile(pSlopeTile->GetX() - 1, pSlopeTile->GetY() - 1);
				if (pOtherTile)
				{
					if (!pOtherTile->IsSlope())
					{
						fPercentX = 0.0f;
					}
				}
			}
		}
	}

	float fSlopeHeightY = (1 - fPercentX) * (pSlopeTile->GetFloorLeftY()) + (fPercentX)* (pSlopeTile->GetFloorRightY());

	float distFromObstacle = fSlopeHeightY - fEntityBottomY;

	// We're going to hit the sloped floor
	if (distFromObstacle < fVelocityY)
	{
		fVelocityY = distFromObstacle;
	}

	// Set the new position
	pPosition->y += fVelocityY;
	pEntity->GetDisplacementController()->SetDirty(true);
}

// Is this entity effected by gravity?
bool MoveController::IsAffectedByGravity() const
{
	if(pEntity == NULL)
		return false;

	if(pEntity->GetDisplacementController() == false)
		return false;

	if (IsGravityDisabled())
		return true;

	if(IsOnLadder())
		return false;

	if (pEntity->GetWorldState() == Entity::WORLD_STATE_GROUND)
		return false;

	return true;
}

bool MoveController::IsGravityDisabled() const
{
	return m_bGravityDisabled;
}

void MoveController::SetGravityDisabled(bool bNewValue)
{
	m_bGravityDisabled = bNewValue;
}

//------------------------------------------------------------

Entity* MoveController::GetEntity() const
{
	return pEntity;
}