#pragma once

#ifndef __ENUMS_H
#define __ENUMS_H

enum ComponentType
{
	COMPONENT_NONE			= 0,
	COMPONENT_DISPLACEMENT	= 1 << 1,	// Entity position
	COMPONENT_MOVEMENT		= 1 << 2,	// Entity movement (like a normal physical object)
	COMPONENT_RENDER		= 1 << 3,	// Entity rendering
	COMPONENT_INPUT			= 1 << 4,	// Entity can accept input (use for players only right now)
	COMPONENT_WEAPON		= 1 << 5,	// Entity has weapons
	COMPONENT_PROJECTILE	= 1 << 6,	// Entity behaves like a projectile
	COMPONENT_HEALTH		= 1 << 7,	// Entity has health

	// Component Presets
	COMPONENT_PLAYER		= COMPONENT_DISPLACEMENT | COMPONENT_MOVEMENT | COMPONENT_RENDER | COMPONENT_INPUT | COMPONENT_WEAPON | COMPONENT_HEALTH,
	COMPONENT_NORMAL		= COMPONENT_DISPLACEMENT | COMPONENT_MOVEMENT | COMPONENT_RENDER
};

enum TileType
{
	TILE_EMPTY,
	TILE_SOLID,				// Completely solid tile; entities cannot move through them at all
	TILE_ONE_WAY,			// Entities can move through this tile from the bottom, but not from the top
	TILE_LADDER,			// Forces entities into a separate "ladder state"
	TILE_SLOPE,				

	NUM_TILE_TYPES
};

enum DirectionType
{
	NO_DIRECTION_TYPE = 0,

	DIRECTION_UP	= 1 << 1,
	DIRECTION_DOWN	= 1 << 2,
	DIRECTION_LEFT	= 1 << 3,
	DIRECTION_RIGHT	= 1 << 4,
};

enum WeaponType
{
	NO_WEAPON_TYPE = -1,

	WEAPON_TYPE_SEMI_AUTO,
	WEAPON_TYPE_FULL_AUTO,

	NUM_WEAPON_TYPES
};

enum ScreenType
{
	NO_SCREEN_TYPE = -1,

	SCREEN_TYPE_MAIN_MENU,		// main menu screen
	SCREEN_TYPE_LOADING,		// Loading screen
	SCREEN_TYPE_INGAME,			// play screen - will always have a level associated with it
	SCREEN_TYPE_PAUSE,			// the pause screen
	SCREEN_TYPE_INVENTORY,		// inventory screen

	NUM_SCREEN_TYPES
};

#endif