#pragma once

#ifndef WEAPON_CONTROLLER_H
#define WEAPON_CONTROLLER_H

#include <list>

#include "Entity.h"
#include "Weapons.h"
#include "Component.h"

// Forward declaration
class Entity;
class Weapon;

//////////////////////////////////////////////////////////////////
// Class: WeaponController
// - Stores all the information about the weapons of an Entity
// - An entity can have multiple weapons, but only one may be active at a time
//////////////////////////////////////////////////////////////////
class WeaponController : public Component
{
public:
	WeaponController(Entity* Entity);
	~WeaponController();

	void Uninit();

	void update();

	bool AddWeapon(WeaponInfo eWeaponInfo, int iCurrentAmmo, int iCurrentMagazine);
	void SwitchWeapon(int iIndex);

	void SetActiveWeapon(int index);
	Weapon* GetActiveWeapon() const;
	std::vector<Weapon*> GetWeapons() const;
private:
	Weapon* m_pActiveWeapon;
	std::vector<Weapon*> m_Weapons;
};

#endif