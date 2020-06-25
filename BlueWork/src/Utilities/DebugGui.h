#pragma once

#include <vector>
#include <functional>

#include <GLFW/glfw3.h>

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>


class DebugGui{
public:
	typedef std::function<void(void)> GuiFunction;
private:
	static std::vector<GuiFunction> DrawFunctions;

public:

	static void Initialize(GLFWwindow* window);

	template<typename Class>
	static void PushDraw(void (Class::* f)(void), Class* source) {
		DrawFunctions.push_back(std::bind(f, static_cast<Class*>(source)));
	}

	static void Render();

	static void CleanUp();


};

