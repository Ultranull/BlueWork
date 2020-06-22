#include "InputComponent.h"


InputComponent::InputComponent(GLFWwindow* window) :
	Window(window) {}

void InputComponent::Initialize() {}
void InputComponent::CleanUp() {}


void InputComponent::SetCursorMode(CursorInputMode mode) {
	glfwSetInputMode(Window, GLFW_CURSOR, static_cast<int>(mode));
}

void InputComponent::GetCursorPostion(double& mouseX, double& mouseY) {
	glfwGetCursorPos(Window, &mouseX, &mouseY);
}

void InputComponent::SetCursorPostion(double mouseX, double mouseY) {
	glfwSetCursorPos(Window, mouseX, mouseX);
}

InputState InputComponent::GetKeyState(int key) {
	return static_cast<InputState>(glfwGetKey(Window, key));
}