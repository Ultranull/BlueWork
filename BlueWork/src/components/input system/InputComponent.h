#pragma once

#include <functional>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "componentSystem/AbstractComponent.h"

#include "InputEnums.h"

class InputSystem;

class InputComponent: public AbstractComponent{
	friend InputSystem;
	GLFWwindow* Window;

	std::function<void(int, int, int, int)> KeyEvent;
	std::function<void(double, double)> MouseMoveEvent;
	
	void mousebuttonevent();

public:

	InputComponent(GLFWwindow* window);

	virtual void Initialize();
	virtual void CleanUp();

	template<typename Class>
	void SetKeyEvent(void(Class::* f)(int, int, int, int), Class* source) {
		KeyEvent = std::bind(
			f, 
			static_cast<Class*>(source), 
			std::placeholders::_1, 
			std::placeholders::_2, 
			std::placeholders::_3, 
			std::placeholders::_4);
	}

	template<typename Class>
	void SetMouseMoveEvent(void(Class::* f)(double, double), Class* source) {
		MouseMoveEvent = std::bind(
			f,
			static_cast<Class*>(source),
			std::placeholders::_1,
			std::placeholders::_2);
	}

	void SetCursorMode(CursorInputMode mode);

	void GetCursorPostion(double& mouseX, double& mouseY);

	void SetCursorPostion(double mouseX, double mouseY);

	InputState GetKeyState(int key);

};

