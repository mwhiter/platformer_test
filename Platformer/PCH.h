#pragma once

#ifndef __PCH_H_
#define __PCH_H_

#include <stack>
#include <vector>
#include <time.h>
#include <math.h>

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "base.h"
#include "enums.h"
#include "data.h"

#include "core.h"

#include "Entity.h"
#include "Level.h"
#include "Tile.h"
#include "Weapons.h"

#include "Component.h"
#include "DisplacementController.h"
#include "RenderController.h"
#include "InputController.h"
#include "MoveController.h"
#include "WeaponController.h"
#include "ProjectileController.h"
#include "Renderer.h"
#include "HealthController.h"

#endif