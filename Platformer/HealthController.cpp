#include "PCH.h"

HealthController::HealthController(Entity* pEntity):
	Component(pEntity),
	m_iHealthTimes100(10000),
	m_iLastHit(-1)
{
}


HealthController::~HealthController()
{
}

void HealthController::reset()
{
	m_iHealthTimes100 = 10000;
	m_iLastHit = -1;
}

int HealthController::GetHealthTimes100()
{
	return m_iHealthTimes100;
}

void HealthController::SetHealthTimes100(int iNewValue)
{
	m_iHealthTimes100 = iNewValue;

	// Trigger death
	if (m_iHealthTimes100 <= 0)
	{
		m_iHealthTimes100 = 0;
		pEntity->kill();
	}
}

int HealthController::GetHealth()
{
	return GetHealthTimes100() / 100;
}

bool HealthController::IsInvulnerable() const
{
	// If we're the player and we were last hit
	// todo: entities can have "temp invulnerability flag"?
	if (pEntity->isPlayer())
		if (m_iLastHit >= 0)
			if (clock() - m_iLastHit < 2000) // todo: some constant...
				return true;

	return false;
}

// Deals damage to the entity times 100.
void HealthController::damageTimes100(int iDamageTimes100)
{
	if (IsInvulnerable()) return;

	SetHealthTimes100(GetHealthTimes100() - iDamageTimes100);
	m_iLastHit = clock();
}

void HealthController::update()
{

}