#pragma once

#include <memory>

#include "graphics/VertexArray.h"


struct Geometry {
	GLuint topology = 0;
	int size = 0;
	int instances = 1; // should this be in here?
	bool indexed = false;
	bool deleted = false;
	std::unique_ptr<VertexArray> vaObject;

	void draw();
	void drawWire();

	void cleanup();
};