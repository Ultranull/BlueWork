#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include "../graphics/Buffer.h"
#include "../graphics/ShaderProgram.h"

#include "Node.h"

class Camera: public Node{

	glm::mat4 view;		  
	glm::mat4 projection; 

	UniformBuffer* buffer;

	float FOV = 45.f;

	float speed = 3.f;
	float mouseSpeed = 0.005f;
	double xpos, ypos;
	float hangle = 0, vangle = 0;
	float orbitDist = 3;

public:
	Camera();

	static std::shared_ptr<UniformBuffer> buildCamera(void);

	void bindCamera(UniformBuffer* buf, Program shader);
	void updateBuffer();

	void apply(GLFWwindow *window, float delta);
	void orbit(GLFWwindow *window, float delta, glm::vec3 target);
	glm::mat4 P();
	glm::mat4 V();
	void perspective(int width, int height, float FOV, float near, float far);
	void orthographic(float left, float right, float bottom, float top, float near, float far);

	glm::vec3 getDirection();
	glm::vec3 getPosition();

	void cleanup();
};
