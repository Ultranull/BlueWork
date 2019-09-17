#pragma once

#include "VertexArray.h"

struct Geometry {
	GLuint topology;
	int size, instances = 1;
	VertexArray* vaObject; // make it a pointer, pass around a mem loc

	void drawArrays();

	void cleanup();
};