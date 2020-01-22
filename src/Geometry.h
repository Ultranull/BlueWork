#pragma once

#include "graphics/VertexArray.h"

struct Geometry {
	GLuint topology;
	int size, instances = 1;
	VertexArray* vaObject;

	void drawArrays();

	void cleanup();
};