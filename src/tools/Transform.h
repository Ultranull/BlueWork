#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

struct Transform {
	glm::vec3 position_m = glm::vec3(0);
	glm::vec3 scale_m = glm::vec3(1);
	glm::quat rotation_m;

	glm::mat4 Model();

	glm::mat4 T();
	glm::mat4 S();
	glm::mat4 R();

	void rotateBy(glm::quat b);
	void rotate(float rads, glm::vec3 axis);
	void translate(glm::vec3 t);
	void scale(glm::vec3 t);
};
