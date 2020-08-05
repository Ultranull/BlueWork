#include "InputSystem.h"


template<>
InputComponent* SystemManager::CreateComponent() {
	SystemInterface* si = systems[InputSystem::Name].get();
	InputSystem* cp = static_cast<InputSystem*>(si);

	return cp->Register();
}

InputSystem::InputSystem(GLFWwindow* window) :
	Window(window),
	Manager(SystemManager::getInstance()),
	AbstractSystem(){
}

void InputSystem::KeyboardCallback(int key, int scancode, int action, int mods) {
	for (int i = 0; i < components.size(); i++) {
		components[i]->KeyEvent(key, scancode, action, mods);
	}
}

void InputSystem::MouseMoveCallback(double xpos, double ypos) {
	for (int i = 0; i < components.size(); i++) {
		components[i]->MouseMoveEvent(xpos, ypos);
	}
}

void InputSystem::OnStart() {
	glfwSetKeyCallback(Window, InputSystem::onKey); 
	glfwSetCursorPosCallback(Window, InputSystem::OnMouseMove);
	glfwSetWindowUserPointer(Window, this);
}

void InputSystem::OnUpdate() {

}

InputComponent* InputSystem::Register() {
	components.push_back(std::make_unique<InputComponent>(Window));
	return components.back().get();
}