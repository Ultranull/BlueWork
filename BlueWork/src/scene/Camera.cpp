#include "Camera.h"

#include<stdio.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "glm/gtc/matrix_transform.hpp"
#include <glm/gtc/type_ptr.hpp>

#include "Utilities/DebugGui.h"

#include "resource/Serializer.h"

using namespace glm;

size_t index_proj = 0;
size_t index_view = index_proj + sizeof(glm::mat4);
size_t index_up = index_view + sizeof(glm::mat4);
size_t index_dir = index_up + sizeof(glm::vec4);
size_t index_pos = index_dir + sizeof(glm::vec4);

Camera::Camera(CameraSettings s):
	settings(s),buffer(nullptr), Node("Camera", NodeType::Camera) {

	DebugGui::PushDraw(&Camera::guidraw, this);
}

Camera::Camera():
	Camera(CameraSettings{
		CameraSettings::Mode::Perspective,
		{.1, 100, 45.f} }){

}

void Camera::bindCamera(UniformBuffer* buf, Program shader){
	buffer = buf;
	buffer->bind();
	buffer->blockBinding(shader.getProgramID(), 0, "camera");
}

std::shared_ptr<UniformBuffer> Camera::buildCamera(void) {
	std::shared_ptr<UniformBuffer> buffer = std::make_shared<UniformBuffer>();
	buffer->bind();
	buffer->setData<mat4>(3, GL_DYNAMIC_DRAW);
	buffer->setSubData(index_proj, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1)));
	buffer->setSubData(index_view, sizeof(glm::mat4), glm::value_ptr(glm::mat4(1)));
	buffer->setSubData(index_up, sizeof(glm::vec4), glm::value_ptr(glm::vec4(1)));
	buffer->setSubData(index_dir, sizeof(glm::vec4), glm::value_ptr(glm::vec4(1)));
	buffer->setSubData(index_pos, sizeof(glm::vec4), glm::value_ptr(glm::vec4(1)));
	buffer->setSubData(index_pos + sizeof(glm::vec4), sizeof(glm::vec4), glm::value_ptr(glm::vec4(1)));
	buffer->unbind();
	return buffer;
}

void Camera::updateBuffer(){

	if (buffer != nullptr) {
		buffer->bind();
		buffer->setSubData(index_proj, sizeof(glm::mat4), glm::value_ptr(P()));
		buffer->setSubData(index_view, sizeof(glm::mat4), glm::value_ptr(V()));
		buffer->setSubData(index_up, sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.FinalUp(), 1)));
		buffer->setSubData(index_dir, sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.FinalForward(), 1)));
		buffer->setSubData(index_pos, sizeof(glm::vec4), glm::value_ptr(glm::vec4(transform.FinalPosition(), 1)));
		buffer->unbind();
	}
}

//void Camera::orbit(GLFWwindow *window, float delta,vec3 target) {
//	int width, height;
//	glfwGetFramebufferSize(window, &width, &height);//FIX! all input should be called from input class
//	glfwGetCursorPos(window, &xpos, &ypos);
//	glfwSetCursorPos(window, width / 2, height / 2);
//	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
//
//
//	if (glfwGetKey(window, FORWARD) == GLFW_PRESS) {//FIX! all input should be called from input class
//		orbitDist -=  delta * speed;
//	}
//	if (glfwGetKey(window, BACKWARD) == GLFW_PRESS) {
//		orbitDist +=  delta * speed;
//	}
//
//	hangle += mouseSpeed * float(width / 2 - xpos);
//	vangle += mouseSpeed * float(height / 2 - ypos);
//
//	float pi = radians(180.f);
//
//	transform.Position() = vec3(cos(vangle)*sin(hangle),
//					 sin(vangle),
//					 cos(vangle)*cos(hangle))*orbitDist + target;
//	vec3 direction = (transform.Position() - target)/ orbitDist;
//	vec3 right(sin(hangle - pi / 2.f),
//			   0,
//			   cos(hangle - pi / 2.f));
//	vec3 up = cross(right, direction);
//
//	float ratio = width / (float)height;
//	view = glm::lookAt(
//		transform.Position(),
//		target,
//		up  
//	);
//
//
//}

void Camera::UpdateProjection(int height, int width) {
	switch (settings.mode){
		case CameraSettings::Mode::Perspective:
			projection = glm::perspective(
				radians(settings.PerspecitveData.FOV),
				width / (float)height, 
				settings.PerspecitveData.NearPlane,
				settings.PerspecitveData.FarPlane);
			break;

		case CameraSettings::Mode::Orthographic:
			projection = glm::ortho(
				settings.OrthographicData.Left,
				settings.OrthographicData.Right,
				settings.OrthographicData.Bottom,
				settings.OrthographicData.Top,
				settings.OrthographicData.NearPlane,
				settings.OrthographicData.FarPlane);
			break;
	}

	glm::mat4 finalTransform = ResolveFinalTransform();
	view = lookAt(
		glm::vec3(finalTransform * glm::vec4(0, 0, 0, 1)),
		glm::vec3(finalTransform * glm::vec4(0, 0, 1, 1)),
		glm::vec3(finalTransform * glm::vec4(0, 1, 0, 0)));
}

void Camera::UpdateProjection(int height, int width, CameraSettings newSettings) {
	settings = newSettings;
	UpdateProjection(height, width);
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

CameraSettings Camera::GetSettings() {
	return settings;
}

void Camera::SetSettings(CameraSettings cs) { settings = cs; }

void Camera::cleanup(){
	//buffer->cleanup();
}

void Camera::guidraw() {
	ImGui::Begin("window"); {
		if (ImGui::TreeNode((name + std::string(":") + std::to_string(Id) + std::string(":") + TypeName).c_str())) {
			ImGui::Indent();
			ImGui::DragFloat3(
				(std::string("position") + std::string("##") + std::to_string(Id)).c_str(),
				reinterpret_cast<float*>(&transform.Position()));
			ImGui::DragFloat4(
				(std::string("rotation") + std::string("##") + std::to_string(Id)).c_str(),
				reinterpret_cast<float*>(&transform.Rotation()));

			ImGui::Unindent();
			ImGui::TreePop();
		}
	}ImGui::End();
}
