#pragma once

// Forward declaration
class Entity;

class Component
{
public:
	Component(Entity* pEntity);
	~Component();

	virtual void update() = 0;
protected:
	Entity* pEntity;
};

