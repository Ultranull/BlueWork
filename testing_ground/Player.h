#pragma once

#include <glm/glm.hpp>

#include "components/input system/InputComponent.h"
#include "scene/Entity.h"

class Player : public Entity {

	InputComponent* input;

	float speed;
	float mouseSpeed;
	double xpos, ypos, oldxpos, oldypos;
	float hangle, vangle;

	bool captureMouse = false;

public:

	Player(std::shared_ptr<Geometry> geometry, Material mat);

	void OnMouseMove(double mouseX, double mouseY);

	void OnKeyEvent(int key, int scancode, int action, int mods);

	void movement(float delta, int width, int height);

	static void RegisterSerializer();
};