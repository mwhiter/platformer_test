#include "PCH.h"

ProjectileController::ProjectileController(Entity* pEntity) :
	Component(pEntity),
	pWeapon(NULL),
	m_eMoveDirection(NO_DIRECTION_TYPE),
	m_Vel(new sf::Vector2f(0.0f,0.0f))
{
}


ProjectileController::~ProjectileController()
{
}

void ProjectileController::SetMoveDirection(DirectionType eDirection)
{
	m_eMoveDirection = eDirection;
}
void ProjectileController::SetVelocity(float x, float y)
{
	m_Vel->x = x;
	m_Vel->y = y;
}

// Update the projectile
void ProjectileController::update()
{
	if (pEntity->IsDelayedDeath()) return;

	pEntity->GetDisplacementController()->ChangePositionX(m_Vel->x);
	pEntity->GetDisplacementController()->ChangePositionY(m_Vel->y);

	// If we're outside the screen, kill the projectile
	if (!core.GetRenderer()->IsVisibleOnScreen(*pEntity->GetDisplacementController()->GetPosition()))
	{
		pEntity->SetDelayedDeath(true);
	}
}

Weapon* ProjectileController::GetWeapon() const
{
	return pWeapon;
}

void ProjectileController::SetWeapon(Weapon* pWeapon)
{
	this->pWeapon = pWeapon;
}