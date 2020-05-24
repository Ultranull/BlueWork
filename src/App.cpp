#include "App.h"

#include <loguru.hpp>

	App::App() {
	}


	App::~App() {
		glfwTerminate();
	}


	void App::viewportinit() {
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);
	}
	App::App(int w, int h, const char *title) {
		LOG_F(INFO, "Loading GLFW...");
		if (!glfwInit()) {
			LOG_F(FATAL, "Failed to initialize glfw");
			exit(-1);
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_SAMPLES, 64);

		LOG_F(INFO, "Creating window...");
 		window = glfwCreateWindow(w, h, title, NULL, NULL);
		if (window == nullptr) {
			LOG_F(FATAL, "Failed to init window");
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(window);

		LOG_F(INFO, "Loading GLAD...");
		int res=gladLoadGL();
		if (!res) {
			LOG_F(FATAL, "Failed to init glad (%i)", res);
			glfwTerminate();
			exit(-1);
		}
		glfwSetWindowUserPointer(window, this);
		//glfwSetKeyCallback(window, onKey);
		viewportinit();
	}

	void App::mainLoop() {

		int frames = 0;
		double lastFrame = glfwGetTime();
		double lastFPS = glfwGetTime();

		LOG_F(INFO, "Starting main loop...");
		while (running&&glfwWindowShouldClose(window) == 0) {
			viewportinit();
			double currenttime = glfwGetTime();
			frames++;
			if (currenttime - lastFPS >= 1) {
				fps = frames;
				frames = 0;
				lastFPS += 1.0;
			}
			double delta = currenttime - lastFrame;
			lastFrame = currenttime;
			inputListener(delta);
			update(delta);
			render(delta);
			ticks += 1 * delta;
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		onClose();
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	void App::start() {
		initGL();
		init();
		mainLoop();
	}