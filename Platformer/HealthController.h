#pragma once

#ifndef HEALTH_CONTROLLER_H
#define HEALTH_CONTROLLER_H

#include "Component.h"
#include "Entity.h"

class HealthController :
	public Component
{
public:
	HealthController(Entity* pEntity);
	~HealthController();

	int GetHealthTimes100();
	void SetHealthTimes100(int iNewValue);
	int GetHealth();

	void damageTimes100(int iDamageTimes100);
	bool IsInvulnerable() const;

	void update();
	void reset();
private:
	int m_iHealthTimes100;
	clock_t m_iLastHit;
};

#endif // HEALTH_CONTROLLER_H
