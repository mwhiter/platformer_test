#include "PCH.h"

WeaponController::WeaponController(Entity* pEntity):
	Component(pEntity),
	m_pActiveWeapon(NULL),
	m_Weapons()
{
}


WeaponController::~WeaponController()
{
	Uninit();
}

void WeaponController::Uninit()
{
	m_pActiveWeapon = NULL;

	// free any memory in the vector
	std::vector<Weapon*>::iterator it;
	for (it = m_Weapons.begin(); it != m_Weapons.end(); it++)
	{
		delete *(it);
	}
	m_Weapons.clear();
}

void WeaponController::update()
{
	if (pEntity->GetFireState() == Entity::FIRE_STATE_SHOOT_PRIMARY)
	{
		Weapon::WeaponError eWeaponError = GetActiveWeapon()->fire(pEntity);

		// If we have a weapon error, we can catch it here and do stuff (play a sound maybe?)
	}
	else if (pEntity->GetFireState() == Entity::NO_FIRE_STATE)
	{
		if (GetActiveWeapon())
		{
			if (GetActiveWeapon()->GetState() == Weapon::STATE_SEMI_NOT_READY)
			{
				GetActiveWeapon()->ChangeState(Weapon::NO_STATE);
			}
		}
	}

	for (std::vector<Weapon*>::size_type i = 0; i < m_Weapons.size(); i++)
	{
		m_Weapons[i]->update();
	}
}

// Add a new weapon to the controller, returns false if weapon wasn't added
bool WeaponController::AddWeapon(WeaponInfo eWeaponInfo, int iCurrentAmmo, int iCurrentMagazine)
{
	bool bSuccess = false;

	// Allocate memory for a new weapon and push it into the array
	Weapon* pNewWeapon = new Weapon(eWeaponInfo, iCurrentMagazine, iCurrentAmmo);
	if (pNewWeapon)
	{
		m_Weapons.push_back(pNewWeapon);
		bSuccess = true;
	}

	if (GetActiveWeapon() == NULL)
		SetActiveWeapon(m_Weapons.size()-1);

	return bSuccess;
}

#pragma warning (disable : 4018)

// Switch our active weapon to iIndex
void WeaponController::SwitchWeapon(int iIndex)
{
	if (m_pActiveWeapon == m_Weapons[iIndex]) return;
	if (iIndex < 0 || iIndex >= m_Weapons.size()) return;
	
	m_pActiveWeapon = m_Weapons[iIndex];
}

void WeaponController::SetActiveWeapon(int index)
{
	if (index >= m_Weapons.size() || index < 0)
		return;

	m_pActiveWeapon = m_Weapons[index];
}

#pragma warning (default : 4018)

Weapon* WeaponController::GetActiveWeapon() const
{
	return m_pActiveWeapon;
}

std::vector<Weapon*> WeaponController::GetWeapons() const
{
	return m_Weapons;
}