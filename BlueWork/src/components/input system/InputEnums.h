#pragma once

#include <GLFW/glfw3.h>

enum class CursorInputMode {
	Normal = GLFW_CURSOR_NORMAL,
	Hidden = GLFW_CURSOR_HIDDEN,
	Disabled = GLFW_CURSOR_DISABLED
};

enum class InputState {
	Pressed = GLFW_PRESS,
	Released = GLFW_RELEASE
};