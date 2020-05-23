#pragma once

#include <glm/glm.hpp>

#include "components/input system/InputComponent.h"
#include "scene/Entity.h"

class Player : public Entity {

	InputComponent* input;

	float speed;
	float mouseSpeed;
	double xpos, ypos;
	float hangle, vangle;

public:

	Player(Geometry* geometry, Material mat);

	void OnMouseMove(double mouseX, double mouseY);

	void movement(float delta, int width, int height);
};