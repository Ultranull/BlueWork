#include "InputSystem.h"


template<>
InputComponent* SystemManager::CreateComponent() {
	SystemInterface* si = systems[InputSystem::Name];
	InputSystem* cp = static_cast<InputSystem*>(si);

	return cp->Register();
}

InputSystem::InputSystem(GLFWwindow* window) :
	Window(window),
	Manager(SystemManager::getInstance()) {
}

void InputSystem::callback(int key, int scancode, int action, int mods) {
	for (int i = 0; i < components.size(); i++) {

	}
}

void InputSystem::OnStart() {

}

void InputSystem::OnUpdate() {

}

InputComponent* InputSystem::Register() {
	components.push_back(InputComponent(Window));
	return &components.back();
}