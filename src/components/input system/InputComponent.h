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

	std::function<void(int, int)> KeyEvent;
	
	void mousebuttonevent();

	void mousemoveevent();

public:

	InputComponent(GLFWwindow* window);

	virtual void Initialize();
	virtual void CleanUp();

	void SetCursorMode(CursorInputMode mode);

	void GetCursorPostion(double& mouseX, double& mouseY);

	void SetCursorPostion(double mouseX, double mouseY);

	InputState GetKeyState(int key);

};

