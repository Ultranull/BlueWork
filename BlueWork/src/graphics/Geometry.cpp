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
			GL_LINE_LOOP, size, GL_UNSIGNED_INT, nullptr, instances, 0, 0);
	else
		glDrawArraysInstancedBaseInstance(
			GL_LINE_LOOP, 0, size, instances, 0);
	vaObject->unbind();
}

void Geometry::cleanup() {
	if (this != nullptr && vaObject != nullptr && !deleted) {
		vaObject->cleanup();
		deleted = true;
	}
}
