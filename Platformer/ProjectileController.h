#pragma once

#ifndef __PROJECTILE_CONTROLLER_H
#define __PROJECTILE_CONTROLLER_H

#include "enums.h"

class ProjectileController : public Component
{
public:
	ProjectileController(Entity* pEntity);
	~ProjectileController();

	void SetMoveDirection(DirectionType eDirection);
	void SetVelocity(float x, float y);

	void update();

	Weapon* GetWeapon() const;
	void SetWeapon(Weapon* pWeapon);
private:
	Weapon* pWeapon;

	DirectionType m_eMoveDirection;
	sf::Vector2f* m_Vel;
};

#endif