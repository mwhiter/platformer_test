#pragma once

#ifndef __ENTITY_H
#define __ENTITY_H

#include "base.h"
#include "DisplacementController.h"
#include "InputController.h"
#include "MoveController.h"
#include "RenderController.h"
#include "WeaponController.h"
#include "Tile.h"
#include "enums.h"

// Forward declarations
class InputController;
class MoveController;
class ProjectileController;
class HealthController;
class Level;

class Entity
{
public:
	enum State
	{
		NO_STATE = -1,

		STATE_DEAD,					// Entity is dead
		STATE_STANDING,				// Entity is standing (on solid ground)
		STATE_CROUCH,				// Entity is crouched
	};

	enum WorldState
	{
		NO_WORLD_STATE = -1,

		WORLD_STATE_GROUND,			// Entity is on the ground
		WORLD_STATE_AIR,			// Entity is in the air
	};

	enum FireState
	{
		NO_FIRE_STATE = -1,

		FIRE_STATE_SHOOT_PRIMARY,	// Fire primary
	};

	Entity(Level* pLevel, uint index, sf::Vector2f size, sf::Uint32 components);
	~Entity(void);
	Entity(const Entity& other);

	void reset();

	DisplacementController* GetDisplacementController() const;
	InputController* GetInputController() const;
	MoveController* GetMoveController() const;
	RenderController* GetRenderController() const;
	WeaponController* GetWeaponController() const;
	ProjectileController* GetProjectileController() const;
	HealthController* GetHealthController() const;

	uint GetIndex() const;

	void SetSize(sf::Vector2f size);

	float GetSizeX() const;
	float GetSizeY() const;

	DirectionType GetFacing() const;
	void SetFacing(DirectionType eDirection);

	float GetRightEdgeX() const;
	float GetLeftEdgeX() const;
	float GetTopEdgeY() const;
	float GetBottomEdgeY() const;
		
	State GetState() const;
	void SetState(State eState);
	WorldState GetWorldState() const;
	void SetWorldState(WorldState eState);
	FireState GetFireState() const;
	void SetFireState(FireState eState);

	void collide(Entity* pEntity);
	void update();
	void kill();
	void respawn();

	bool IsHostile() const;
	void SetIsHostile(bool bValue);

	bool IsOnSlope() const;
	void SetOnSlope(bool bValue);

	void SetDelayedDeath(bool bValue);
	bool IsDelayedDeath() const;

	Level* GetLevel() const;
	sf::Shape* GetShape();

	// Helper functions
	bool isPlayer() const;
	bool isProjectile() const;
private:
	uint index;
	
	// Components
	DisplacementController* displacement;
	InputController* input;
	MoveController* movement;
	RenderController* render;
	WeaponController* weapons;
	ProjectileController* projectile;
	HealthController* health;

	// States
	State m_eState;
	WorldState m_eWorldState;
	FireState m_eFireState;

	DirectionType m_eFacing;
	bool bOnSlope;
	bool m_bIsHostile;
	
	sf::Vector2f m_size;

	sf::Shape* shape;
	Level* pLevel;

	bool bDelayedDeath;
};

#endif