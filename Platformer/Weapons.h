#pragma once

#ifndef WEAPONS_H
#define WEAPONS_H

//////////////////////////////////////////////////////////////////
// Class: Weapon
// - Stores information about a single weapon in the game
// - Each weapon has it's own special properties
//////////////////////////////////////////////////////////////////
class Weapon
{
public:
	enum WeaponError
	{
		NO_WEAPON_ERROR = -1,

		WEAPON_ERROR_NO_DIRECTION,
		WEAPON_ERROR_OUT_OF_AMMO,
		WEAPON_ERROR_MAGAZINE_EMPTY,
		WEAPON_ERROR_TOO_SOON,
		WEAPON_ERROR_RELOADING
	};

	enum State
	{
		NO_STATE = -1,

		STATE_RELOADING,
		STATE_SEMI_NOT_READY,
	};

	Weapon(WeaponInfo eWeaponInfo, int iCurrentMagazine, int iCurrentAmmo);
	~Weapon();

	void update();

	WeaponError fire(Entity* pEntity);
	void stop();
	
	WeaponType GetType() const;
	std::string GetName() const;
	std::string GetSoundFile() const;

	bool IsReadyToShoot() const;

	void startReload();
	void finishReload();

	int GetTotalGunAmmo() const;
	int GetCurrentMagazineAmmo() const;
	int GetCurrentGunAmmo() const;

	void ChangeState(State eState);
	State GetState() const;

	int GetRPM() const;
	int GetMagazineSize() const;
	int GetMaxAmmo() const;
	int GetProjectileSpeed() const;
	int GetDamage() const;
private:
	void ReadWeaponData(WeaponInfo eWeaponInfo);
	clock_t GetTimeBetweenFire() const;
	void shoot(Entity* pEntity);

	sf::Vector2f GetProjectilePosition(Entity* pEntity, DirectionType eDirection);

	WeaponType m_eWeaponType;

	// Current Weapon stuff - to-do, weapon controller instead???
	int m_iCurrentMagAmmo;		// how much ammo is currently in the magazine
	int m_iCurrentGunAmmo;		// how much ammo is currently left in the gun
	
	std::string m_szName;		// weapon name
	std::string m_szSoundFile;	// sound file

	// Weapon stats
	int m_iRPM;					// how fast the weapon (in rounds/minute)
	int m_iMagazineSize;		// how many bullets a magazine has
	int m_iMaxAmmo;				// the maximum ammo the weapon has
	int m_iProjectileSpeed;		// how fast the projectiles are fired (in m/s)
	int m_iDamage;				// how much damage we do per shot
	int m_iReloadSpeed;			// reload speed in ms

	// State
	State m_eState;

	clock_t m_iLastBulletTime;	// Time since we fired our last bullet
	clock_t m_iReloadStarted;	// Time since reload started
};

#endif