#include "PCH.h"
#include <functional>

// Weapon constructor
Weapon::Weapon(WeaponInfo eWeaponInfo, int iCurrentMagazine, int iCurrentAmmo) :
	m_iLastBulletTime(0),
	m_iReloadStarted(0),
	m_eState(NO_STATE)
{
	ReadWeaponData(eWeaponInfo);

	// Place ammunition into the weapon
	m_iCurrentMagAmmo = std::min(iCurrentMagazine, m_iMagazineSize);
	m_iCurrentGunAmmo = std::min(iCurrentAmmo, m_iMaxAmmo);
}

Weapon::~Weapon()
{

}

void Weapon::ReadWeaponData(WeaponInfo eWeaponInfo)
{

	m_szName			= eWeaponInfo.name;
	m_szSoundFile		= eWeaponInfo.sound;
	m_eWeaponType		= eWeaponInfo.weaponType;
	m_iRPM				= eWeaponInfo.rpm;
	m_iMaxAmmo			= eWeaponInfo.maxammo;
	m_iMagazineSize		= eWeaponInfo.magsize;
	m_iProjectileSpeed	= eWeaponInfo.projSpeed;
	m_iDamage			= eWeaponInfo.damage;
	m_iReloadSpeed		= eWeaponInfo.reloadSpeedMS;
}

void Weapon::update()
{
	clock_t currentTime = clock();

	if (GetState() == Weapon::STATE_RELOADING)
	{
		if (clock() - m_iReloadSpeed >= m_iReloadStarted)
		{
			finishReload();
		}
	}
}

// Fires the weapon
Weapon::WeaponError Weapon::fire(Entity* pEntity)
{
	if (pEntity->GetFacing() == NO_DIRECTION_TYPE)
		return WEAPON_ERROR_NO_DIRECTION;
	if (GetTotalGunAmmo() == 0)
		return WEAPON_ERROR_OUT_OF_AMMO;
	if (GetCurrentMagazineAmmo() == 0)
	{
		startReload();
		return WEAPON_ERROR_MAGAZINE_EMPTY;
	}
	if (GetState() == Weapon::STATE_RELOADING)
	{
		return WEAPON_ERROR_RELOADING;
	}
	if (m_iLastBulletTime > 0)
	{
		if (clock() - m_iLastBulletTime < GetTimeBetweenFire())
			return WEAPON_ERROR_TOO_SOON;
	}
	if (!IsReadyToShoot())
	{
		return WEAPON_ERROR_TOO_SOON;
	}

	shoot(pEntity);

	return NO_WEAPON_ERROR;
}

// Reload the weapon
void Weapon::startReload()
{
	if (GetState() == Weapon::STATE_RELOADING) return;
	if (GetCurrentGunAmmo() == 0) return;
	if (GetCurrentMagazineAmmo() == GetMagazineSize()) return;

	m_iReloadStarted = clock();
	ChangeState(Weapon::STATE_RELOADING);
}

void Weapon::finishReload()
{
	if (GetState() != Weapon::STATE_RELOADING) return;

	// The new magazine size is minimum of the maximum magazine size and the amount of ammo we have remaining
	int iTotalBullets = m_iCurrentGunAmmo + m_iCurrentMagAmmo;
	int iNewMagSize = std::min(m_iMagazineSize, iTotalBullets);

	m_iCurrentGunAmmo -= m_iMagazineSize - m_iCurrentMagAmmo;
	m_iCurrentGunAmmo = std::max(0, m_iCurrentGunAmmo);
	m_iCurrentMagAmmo = iNewMagSize;

	ChangeState(Weapon::NO_STATE);
}

// Actually shoot the gun. Once we get there, the bullet comes out regardless. Private, so only fire() should call this!
void Weapon::shoot(Entity* pEntity)
{
	Level* pLevel = core.GetCurrentLevel();
	if (pLevel == NULL) return;

	// we need to get projectile parameters...how? Figure that out later. Let's create a dummy projectile for now
	sf::Vector2f projSize(2.0f,1.0f);

	Entity* pProjectile = pLevel->addEntity(projSize, COMPONENT_DISPLACEMENT | COMPONENT_PROJECTILE | COMPONENT_RENDER);
	pProjectile->GetProjectileController()->SetWeapon(this);

	// We successfully allocated memory for the projectile, therefore we are good to go.
	if (pProjectile)
	{
		float fVelocityX = 0.0f;
		if ((pEntity->GetFacing() & DIRECTION_LEFT) == DIRECTION_LEFT)
		{
			fVelocityX = (float)-m_iProjectileSpeed;
		}
		else if ((pEntity->GetFacing() & DIRECTION_RIGHT) == DIRECTION_RIGHT)
		{
			fVelocityX = (float)m_iProjectileSpeed;
		}

		float fVelocityY = 0.0f;
		if ((pEntity->GetFacing() & DIRECTION_UP) == DIRECTION_UP)
			fVelocityY = (float)-m_iProjectileSpeed;
		else if ((pEntity->GetFacing() & DIRECTION_DOWN) == DIRECTION_DOWN)
			fVelocityY = (float)m_iProjectileSpeed;
		
		sf::Vector2f posVector = GetProjectilePosition(pEntity, pEntity->GetFacing());

		pProjectile->GetDisplacementController()->SetPositionX(posVector.x);
		pProjectile->GetDisplacementController()->SetPositionY(posVector.y);
		pProjectile->GetProjectileController()->SetVelocity(fVelocityX, fVelocityY);

		// Subtract ammo from the current magazine. When we reload, we actually subtract from the actual total.
		m_iCurrentMagAmmo--;
		m_iLastBulletTime = clock();

		// If we are a semi-automatic weapon, set that we are not ready
		if (GetType() == WEAPON_TYPE_SEMI_AUTO)
		{
			ChangeState(Weapon::STATE_SEMI_NOT_READY);
		}
	}
}

// Called when the weapon stops firing.
void Weapon::stop()
{
	
}

// Where should we place the projectile?
sf::Vector2f Weapon::GetProjectilePosition(Entity* pEntity, DirectionType eDirection)
{
	if (eDirection == NO_DIRECTION_TYPE) return sf::Vector2f(0.0, 0.0f);
	if (pEntity == NULL) return sf::Vector2f(0.0f, 0.0f);
	if (pEntity->GetDisplacementController() == NULL) return sf::Vector2f(0.0f, 0.0f);

	bool bFacingRight	= (eDirection & DIRECTION_RIGHT) == DIRECTION_RIGHT;
	bool bFacingLeft	= (eDirection & DIRECTION_LEFT) == DIRECTION_LEFT;
	bool bFacingUp		= (eDirection & DIRECTION_UP) == DIRECTION_UP;
	bool bFacingDown	= (eDirection & DIRECTION_DOWN) == DIRECTION_DOWN;

	// Defaults are the center of the respective axes
	float x = pEntity->GetDisplacementController()->GetPosition()->x;
	float y = pEntity->GetDisplacementController()->GetPosition()->y;

	// If they need to be updated they will here
	if (bFacingRight)	x = pEntity->GetRightEdgeX();
	if (bFacingLeft)	x = pEntity->GetLeftEdgeX();
	if (bFacingUp)		y = pEntity->GetTopEdgeY();
	if (bFacingDown)	y = pEntity->GetBottomEdgeY();

	return sf::Vector2f(x, y);
}

// Are we ready to shoot (based on weapon parameters?
bool Weapon::IsReadyToShoot() const
{
	// If we're full auto, always ready to shoot
	if (GetType() == WEAPON_TYPE_FULL_AUTO)
		return true;
	// Semi-automatic weapons must release the mouse in order to shoot again
	if (GetType() == WEAPON_TYPE_SEMI_AUTO)
	{
		if (GetState() == Weapon::STATE_SEMI_NOT_READY)
		{
			return false;
		}
	}

	return true;
}

WeaponType Weapon::GetType() const
{
	return m_eWeaponType;
}
// How many milliseconds must we wait before we can fire again?
clock_t Weapon::GetTimeBetweenFire() const
{
	if (GetRPM() == 0) return 0;
	return clock_t(60 * 1000 / GetRPM());
}
std::string Weapon::GetName() const
{
	return m_szName;
}
std::string Weapon::GetSoundFile() const
{
	return m_szSoundFile;
}
int Weapon::GetTotalGunAmmo() const
{
	return GetCurrentMagazineAmmo() + GetCurrentGunAmmo();
}
int Weapon::GetCurrentMagazineAmmo() const
{
	return m_iCurrentMagAmmo;
}
int Weapon::GetCurrentGunAmmo() const
{
	return m_iCurrentGunAmmo;
}
int Weapon::GetRPM() const
{
	return m_iRPM;
}
int Weapon::GetMagazineSize() const
{
	return m_iMagazineSize;
}
int Weapon::GetMaxAmmo() const
{
	return m_iMaxAmmo;
}
int Weapon::GetProjectileSpeed() const
{
	return m_iProjectileSpeed;
}
int Weapon::GetDamage() const
{
	return m_iDamage;
}
void Weapon::ChangeState(Weapon::State eState)
{
	m_eState = eState;
}
Weapon::State Weapon::GetState() const
{
	return m_eState;
}