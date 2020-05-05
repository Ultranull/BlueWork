#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/quaternion.hpp>

class Transform {
	glm::vec3 _Position = glm::vec3(0);
	glm::vec3 _Scale = glm::vec3(1);
	glm::quat _Rotation;

	bool ValidFinalTransform;
	glm::mat4 _FinalTransform;

public:

	glm::mat4 FinalTransform(bool force, glm::mat4 parent = glm::mat4(1));

	glm::mat4 Model();

	glm::mat4 T();
	glm::mat4 S();
	glm::mat4 R();

	glm::vec3 Up();
	glm::vec3 Forward();

	glm::vec3& Position();
	const glm::vec3& Position() const;
	glm::vec3& Scale();
	const glm::vec3& Scale() const;
	glm::quat& Rotation();
	const glm::quat& Rotation() const;

	void rotateBy(glm::quat b);
	void rotate(float rads, glm::vec3 axis);
	void translate(glm::vec3 t);
	void scale(glm::vec3 t);

	bool IsValid(void);
	
};
