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

	static void Initialize(GLFWwindow* window) {
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = NULL;
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 130");
		ImGui::StyleColorsDark();
	}

	template<typename Class>
	static void PushDraw(void (Class::* f)(void), Class* source) {
		DrawFunctions.push_back(std::bind(f, static_cast<Class*>(source)));
	}

	static void Render() {
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		for (GuiFunction& draw : DrawFunctions) {
			draw();
		}

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	static void CleanUp() {
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}


};

