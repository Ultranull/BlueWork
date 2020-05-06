#include "Camera.h"

#include<stdio.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

using namespace glm;

const GLuint FORWARD = GLFW_KEY_W;//FIX! move to input class
const GLuint BACKWARD = GLFW_KEY_S;
const GLuint STRAFERIGHT = GLFW_KEY_D;
const GLuint STRAFELEFT = GLFW_KEY_A;
const GLuint JUMP = GLFW_KEY_SPACE;
const GLuint CROUCH = GLFW_KEY_LEFT_SHIFT;


Camera::Camera():
	Node(NodeType::Camera){
}

void Camera::bindCamera(Program shader){

	buffer = UniformBuffer();
	buffer.bind();
	buffer.setData<mat4>(3, GL_DYNAMIC_DRAW);
	buffer.setSubData(index_proj, sizeof(glm::mat4), glm::value_ptr(P()));
	buffer.setSubData(index_view, sizeof(glm::mat4), glm::value_ptr(V()));
	buffer.setSubData(index_up  , sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.Up(),0)));
	buffer.setSubData(index_dir , sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.Forward(),0)));
	buffer.setSubData(index_pos , sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.Position(),0)));
	buffer.setSubData(index_pos + sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec4(1)));
	buffer.blockBinding(shader.getProgramID(), 0, "camera");
	buffer.unbind();
}

void Camera::updateBuffer(){
	buffer.bind();
	buffer.setSubData(index_proj, sizeof(glm::mat4), glm::value_ptr(P()));
	buffer.setSubData(index_view, sizeof(glm::mat4), glm::value_ptr(V()));
	buffer.setSubData(index_up  , sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.Up(),0)));
	buffer.setSubData(index_dir , sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.Forward(),0)));
	buffer.setSubData(index_pos , sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.Position(),0)));
	buffer.unbind();
}

void Camera::apply(GLFWwindow *window, float delta) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);//FIX! all input should be called from input class
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, width / 2, height / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	
	hangle += mouseSpeed * float(width / 2 - xpos);//theta
	vangle += mouseSpeed * float(height / 2 - ypos);//phi

	float pi = radians(180.f);

	vec3 direction = vec3(cos(vangle)*sin(hangle),
		sin(vangle),
		cos(vangle)*cos(hangle));
	vec3 right(sin(hangle - pi / 2.f),
		0,
		cos(hangle - pi / 2.f));
	vec3 up = cross(right, direction);
	vec3 front = cross(vec3(0,1,0), right);

	if (glfwGetKey(window, FORWARD) == GLFW_PRESS) {//FIX! all input should be called from input class
		transform.Position() += front * delta * speed;
	}
	if (glfwGetKey(window, BACKWARD) == GLFW_PRESS) {
		transform.Position() -= front * delta * speed;
	}
	if (glfwGetKey(window, STRAFERIGHT) == GLFW_PRESS) {
		transform.Position() += right * delta * speed;
	}
	if (glfwGetKey(window, STRAFELEFT) == GLFW_PRESS) {
		transform.Position() -= right * delta * speed;
	}
	if (glfwGetKey(window, JUMP) == GLFW_PRESS){
		transform.Position() += vec3(0,1,0) * delta * speed;
	}
	if (glfwGetKey(window, CROUCH) == GLFW_PRESS) {
		transform.Position() -= vec3(0, 1, 0) * delta * speed;
	}

	float ratio = width / (float)height;
	view = lookAt(transform.Position(), transform.Position() + direction, up);
}
void Camera::orbit(GLFWwindow *window, float delta,vec3 target) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);//FIX! all input should be called from input class
	glfwGetCursorPos(window, &xpos, &ypos);
	glfwSetCursorPos(window, width / 2, height / 2);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);


	if (glfwGetKey(window, FORWARD) == GLFW_PRESS) {//FIX! all input should be called from input class
		orbitDist -=  delta * speed;
	}
	if (glfwGetKey(window, BACKWARD) == GLFW_PRESS) {
		orbitDist +=  delta * speed;
	}

	hangle += mouseSpeed * float(width / 2 - xpos);
	vangle += mouseSpeed * float(height / 2 - ypos);

	float pi = radians(180.f);

	transform.Position() = vec3(cos(vangle)*sin(hangle),
					 sin(vangle),
					 cos(vangle)*cos(hangle))*orbitDist + target;
	vec3 direction = (transform.Position() - target)/ orbitDist;
	vec3 right(sin(hangle - pi / 2.f),
			   0,
			   cos(hangle - pi / 2.f));
	vec3 up = cross(right, direction);

	float ratio = width / (float)height;
	view = glm::lookAt(
		transform.Position(),
		target,
		up  
	);


}

void Camera::perspective(GLFWwindow *window,float FOV,float near,float far) {
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	projection=glm::perspective(radians(FOV), width / (float)height, near, far);
}

void Camera::orthographic(float left, float right, float bottom, float top, float near, float far) {
	projection = glm::ortho(left,right,bottom,top,near,far);
}
mat4 Camera::P() {
	return projection;
}

mat4 Camera::V() {
	return view;
}

vec3 Camera::getDirection() {
	return transform.Forward();
}

vec3 Camera::getPosition() {
	return transform.Position();
}

void Camera::cleanup(){
	buffer.cleanup();
}