#include "Geometry.h"

#include <glad/glad.h>

void Geometry::draw() {
	vaObject->bind();
	if(indexed)
		glDrawElementsInstancedBaseVertexBaseInstance(
			topology, size, GL_UNSIGNED_INT, nullptr, instances, 0, 0);
	else
		glDrawArraysInstancedBaseInstance(
			topology, 0, size, instances, 0);
	vaObject->unbind();
}

void Geometry::cleanup() {
	vaObject->cleanup();
}
