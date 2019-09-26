#pragma once

#include <vector>


#include "Buffer.h"
#include "Node.h"
#include "Light.h"
#include "ShaderProgram.h"
#include "Entity.h"
#include "FrameBuffer.h"
#include "Mesh.h"

struct Renderer {
	UniformBuffer lights;
	std::vector<lights::Point> lightbuf;

	Node* scene;

	FrameBuffer passthrough;
	Mesh plane;
	Program pass;

	GLuint width, height;

	Renderer();

	Renderer(Program passthrough, GLuint w, GLuint h);

	void cleanup();

	void collectLights(Node* root, glm::mat4 transform);

	void setup(Node* root);

	void updateLights();

	void pre();

	void render();
	void traverseGraph(Node* root, glm::mat4 transform);

	void post();
};