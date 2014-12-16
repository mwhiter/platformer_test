#pragma once

#ifndef __RENDER_CONTROLLER_H
#define __RENDER_CONTROLLER_H

#include "Component.h"
#include "Entity.h"

class RenderController : public Component
{
public:
	RenderController(Entity* pEntity);
	~RenderController(void);

	void draw();
	void update();
};

#endif
