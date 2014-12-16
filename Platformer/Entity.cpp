#include "PCH.h"

Entity::Entity(Level* pLevel, uint index, sf::Vector2f size, sf::Uint32 components) :
	pLevel(pLevel),
	index(index),
	m_size(size),
	m_eFacing(DIRECTION_RIGHT),
	bOnSlope(false),
	bDelayedDeath(false),
	m_bIsHostile(false),
	
	// States
	m_eState(NO_STATE),
	m_eWorldState(NO_WORLD_STATE),
	m_eFireState(NO_FIRE_STATE),

	// Components
	displacement(NULL),
	input(NULL),
	movement(NULL),
	render(NULL),
	weapons(NULL),
	projectile(NULL),
	health(NULL)
{
	// Give components to the entity
	if ((components & COMPONENT_DISPLACEMENT) == COMPONENT_DISPLACEMENT)		displacement	= new DisplacementController(this);
	if ((components & COMPONENT_INPUT) == COMPONENT_INPUT)						input			= new InputController(this);
	if ((components & COMPONENT_MOVEMENT) == COMPONENT_MOVEMENT)				movement		= new MoveController(this);
	if ((components & COMPONENT_RENDER) == COMPONENT_RENDER)					render			= new RenderController(this);
	if ((components & COMPONENT_WEAPON) == COMPONENT_WEAPON)					weapons			= new WeaponController(this);
	if ((components & COMPONENT_PROJECTILE) == COMPONENT_PROJECTILE)			projectile		= new ProjectileController(this);
	if ((components & COMPONENT_HEALTH) == COMPONENT_HEALTH)					health			= new HealthController(this);
}

Entity::~Entity(void)
{
	if (displacement != NULL)	delete displacement;
	if (input != NULL)			delete input;
	if (movement != NULL)		delete movement;
	if (render != NULL)			delete render;
	if (weapons != NULL)		delete weapons;
	if (projectile != NULL)		delete projectile;
	if (health != NULL)			delete health;
}

// Copy constructor - this isn't good enough (yet)
Entity::Entity(const Entity& other)
{
	// Need to fetch a unique id, even for copies.
	index = other.index;

	// Create components (if they exist!)
	if (other.displacement != NULL)	displacement = other.displacement;
	if (other.input != NULL)		input = other.input;
	if (other.movement != NULL)		movement = other.movement;
	if (other.render != NULL)		render = other.render;
	if (other.weapons != NULL)		weapons = other.weapons;
	if (other.projectile != NULL)	projectile = other.projectile;
}

// Update function. Please make sure to check if the component exists before applying functions on it!
void Entity::update()
{
	if (movement != NULL)		movement->update();			// Update its movement
	if (projectile != NULL)		projectile->update();		// Update its projectile movement (don't mix with movement please!)
	if (displacement != NULL)	displacement->update();		// Update its position
	if (weapons != NULL)		weapons->update();			// Update our weapons
}

// reset the entity without losing its components and stuff
void Entity::reset()
{
	m_eFacing = DIRECTION_RIGHT;
	m_eState = NO_STATE;
	m_eWorldState = NO_WORLD_STATE;
	bDelayedDeath = false;

	if (health != NULL)	health->reset();
}

// Entity collides with pEntity
void Entity::collide(Entity* pEntity)
{
	// We are the player
	if (isPlayer())
	{
		// The entity is hostile
		if (pEntity->IsHostile())
		{
			HealthController* pHealth = GetHealthController();
			if (pHealth)
			{
				pHealth->damageTimes100(1000);
			}
		}
	}
	// We are a projectile?
	else if (isProjectile())
	{
		// The entity is hostile
		if (pEntity->IsHostile())
		{
			HealthController* pHealth = pEntity->GetHealthController();
			if (pHealth)
			{
				int iDamageTimes100 = GetProjectileController()->GetWeapon()->GetDamage() * 100;

				pHealth->damageTimes100(iDamageTimes100);
			}
		}

		// Whenever a projectile collides with something, kill it
		SetDelayedDeath(true);
	}
}

// Kill this entity
void Entity::kill()
{
	m_eState = STATE_DEAD;
	m_eWorldState = NO_WORLD_STATE;

	bDelayedDeath = true;
}

// Respawn this player
void Entity::respawn()
{
	reset();
	pLevel->moveToPosition(this, pLevel->GetStartPositions()[0]);

	if (GetHealthController())	GetHealthController()->SetHealthTimes100(10000);
}

DisplacementController* Entity::GetDisplacementController() const
{
	return displacement;
}

InputController* Entity::GetInputController() const
{
	return input;
}

MoveController* Entity::GetMoveController() const
{
	return movement;
}

RenderController* Entity::GetRenderController() const
{
	return render;
}

WeaponController* Entity::GetWeaponController() const
{
	return weapons;
}

ProjectileController* Entity::GetProjectileController() const
{
	return projectile;
}
HealthController* Entity::GetHealthController() const
{
	return health;
}

uint Entity::GetIndex() const
{
	return index;
}

void Entity::SetSize(sf::Vector2f size)
{
	m_size = size;
}

float Entity::GetSizeX() const
{
	return m_size.x;
}

float Entity::GetSizeY() const
{
	return m_size.y;
}

DirectionType Entity::GetFacing() const
{
	// Special case: If we are standing still and looking up, then looking up only
	if (GetMoveController() != NULL)
	{
		if (GetMoveController()->GetMoveState() == MoveController::NO_MOVE_STATE)
		{
			if ((m_eFacing & DIRECTION_UP) == DIRECTION_UP)
			{
				return DIRECTION_UP;
			}
		}
	}

	return m_eFacing;
}

void Entity::SetFacing(DirectionType eDirection)
{

	if (m_eFacing != eDirection)
		m_eFacing = eDirection;
}

bool Entity::IsHostile() const
{
	if (this == pLevel->GetPlayer()) return false;
	return m_bIsHostile;
}

void Entity::SetIsHostile(bool bValue)
{
	m_bIsHostile = bValue;
}

// Are we on a slope?
bool Entity::IsOnSlope() const
{
	return bOnSlope;
}

// Sets if we are on a slope or not
void Entity::SetOnSlope(bool bValue)
{
	if (bOnSlope != bValue)
		bOnSlope = bValue;
}

Entity::State Entity::GetState() const
{
	return m_eState;
}
void Entity::SetState(Entity::State eState)
{
	Entity::State eOldState = m_eState;

	if (m_eState != eState)
	{
		m_eState = eState;
	}
}

Entity::WorldState Entity::GetWorldState() const
{
	return m_eWorldState;
}

void Entity::SetWorldState(Entity::WorldState eState)
{
	if (m_eWorldState != eState)
	{
		m_eWorldState = eState;
	}
}

Entity::FireState Entity::GetFireState() const
{
	return m_eFireState;
}

void Entity::SetFireState(Entity::FireState eState)
{
	if (m_eFireState != eState)
	{
		FireState eOldState = m_eFireState;
		
		// We stopped firing, tell the weapon so we might want to do something
		if (eOldState == FIRE_STATE_SHOOT_PRIMARY &&
			eState == NO_FIRE_STATE)
		{
			if (GetWeaponController())
				GetWeaponController()->GetActiveWeapon()->stop();
		}

		m_eFireState = eState;
	}
}

// Return the entity's right edge
float Entity::GetRightEdgeX() const
{
	if(GetDisplacementController()->GetPosition() == NULL) return 0.0f;
	return (GetDisplacementController()->GetPosition()->x + GetSizeX()/2);
}

float Entity::GetLeftEdgeX() const
{
	if(GetDisplacementController()->GetPosition() == NULL) return 0.0f;
	return (GetDisplacementController()->GetPosition()->x - GetSizeX()/2);
}

// Return the entity's top edge
float Entity::GetTopEdgeY() const
{
	if(GetDisplacementController()->GetPosition() == NULL) return 0.0f;
	return (GetDisplacementController()->GetPosition()->y - GetSizeY()/2);
}

// Return the entity's bottom edge
float Entity::GetBottomEdgeY() const
{
	if(GetDisplacementController()->GetPosition() == NULL) return 0.0f;

	return (GetDisplacementController()->GetPosition()->y + GetSizeY()/2);
}

sf::Shape* Entity::GetShape()
{
	return shape;
}

void Entity::SetDelayedDeath(bool bValue)
{
	bDelayedDeath = bValue;
}

bool Entity::IsDelayedDeath() const
{
	return bDelayedDeath;
}

Level* Entity::GetLevel() const
{
	return pLevel;
}

bool Entity::isPlayer() const
{
	return this == pLevel->GetPlayer();
}

bool Entity::isProjectile() const
{
	return projectile != NULL;
}