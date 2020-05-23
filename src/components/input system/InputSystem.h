#pragma once

#include <string>

#include <GLFW/glfw3.h>

#include "componentSystem/AbstractSystem.h"
#include "componentSystem/SystemManager.h"

#include "InputComponent.h"

class InputSystem :
	public AbstractSystem<InputComponent>{

	SystemManager& Manager;

	GLFWwindow* Window;

	static void onKey(GLFWwindow* window, int key, int scancode, int actions, int mods) {
		InputSystem* obj = static_cast<InputSystem*>(glfwGetWindowUserPointer(window));
		obj->callback(key, scancode, actions, mods);
	}

	void callback(int key, int scancode, int action, int mods);

public:
	inline static const std::string Name = "InputSystem";

	InputSystem(GLFWwindow* window);

	virtual void OnStart();

	virtual void OnUpdate();

	virtual InputComponent* Register();
};
