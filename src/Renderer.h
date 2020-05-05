#pragma once

#include <vector>


#include "graphics/Buffer.h"
#include "scene/Node.h"
#include "scene/Light.h"
#include "graphics/ShaderProgram.h"
#include "scene/Entity.h"
#include "graphics/FrameBuffer.h"
#include "Mesh.h"


/*
	steps:
		-resolve all transforms
		-collect list of each node type needed
		-bind lights
		-loop over entities and render


*/


struct Renderer {
	UniformBuffer lights;
	std::vector<Light::PointData> lightbuf;

	FrameBuffer passthrough;
	Geometry* plane;
	Program pass;

	SceneManager* Manager;

	GLuint width, height;

	Renderer();

	Renderer(Program passthrough, GLuint w, GLuint h);

	void cleanup();

	void collectLights(void);

	void setup(SceneManager* manager);

	void updateLights();

	void pre();

	void render();
	void traverseGraph();

	void post();
};