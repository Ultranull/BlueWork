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

void Geometry::drawWire() {
	vaObject->bind();
	if (indexed)
		glDrawElementsInstancedBaseVertexBaseInstance(
			GL_LINES, size, GL_UNSIGNED_INT, nullptr, instances, 0, 0);
	else
		glDrawArraysInstancedBaseInstance(
			GL_LINES, 0, size, instances, 0);
	vaObject->unbind();
}

void Geometry::cleanup() {
	if (vaObject != nullptr && !deleted) {
		vaObject->cleanup();
		delete vaObject;
		deleted = true;
	}
}
