#pragma once

#include <vector>


#include "Buffer.h"
#include "Node.h"
#include "Light.h"
#include "ShaderProgram.h"
#include "Entity.h"

struct Renderer {
	UniformBuffer lights;
	std::vector<lights::Point> lightbuf;

	Node* scene;

	void cleanup();

	void collectLights(Node* root, glm::mat4 transform);

	void setup(Node* root);

	void updateLights();

	void render();
	void render(Node* root, glm::mat4 transform);
};