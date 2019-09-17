#include "Transform.h"

 glm::mat4 Transform::Model() {
	return T() * R() * S();
}

 glm::mat4 Transform::T() {
	return glm::translate(position_m);
}

 glm::mat4 Transform::S() {
	return glm::scale(scale_m);
}

 glm::mat4 Transform::R() {
	return glm::toMat4(rotation_m);
}

 void Transform::rotateBy(glm::quat b) {
	 rotation_m = b * rotation_m;
 }

 void Transform::rotate(float rads, glm::vec3 axis) {
	 rotation_m = glm::angleAxis(rads, axis);
 }

 void Transform::translate(glm::vec3 t) {
	 position_m = t;
 }

 void Transform::scale(glm::vec3 t) {
	 scale_m = t;
 }
