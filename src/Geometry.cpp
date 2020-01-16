#include "Geometry.h"

#include <glad/glad.h>

void Geometry::drawArrays() {
	vaObject->bind();
	glDrawArrays(topology, 0, size);
	vaObject->unbind();
}

void Geometry::cleanup() {
	vaObject->cleanup();
}
