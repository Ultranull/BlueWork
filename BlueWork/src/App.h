#pragma once

#include <stdio.h>
#include <map>
#include <functional>

#include <windows.h>
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

	class App {
	protected:
		GLFWwindow *window;
		float ticks = 0;
		int fps = 0;
		bool running = true;
		int width, height;

		void viewportinit();

		virtual void init() = 0;
		virtual void initGL() = 0;
		virtual void onClose() = 0;

		virtual void update(float delta) = 0;
		virtual void render(float delta) = 0;
		virtual void inputListener(float delta) = 0;
	public:

		App();
		App(int w, int h, const char *title);
		~App();

		void mainLoop();

		void setSize() {}
		void setPosition() {}

		void start();

	};
/*

* App:
*	-render
*	-init
*	-setIcon
*	-fps_utils
*	-
* 
* 
*	
* 
* */
