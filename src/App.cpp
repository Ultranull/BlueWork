#include "App.h"



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
		if (!glfwInit()) {
			fprintf(stderr, "Failed to initialize glfw\n");
			getchar();
			exit(-1);
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_SAMPLES, 64);
 		window = glfwCreateWindow(w, h, title, NULL, NULL);
		if (window == nullptr) {
			fprintf(stderr, "Failed to init window\n");
			getchar();
			glfwTerminate();
			exit(-1);
		}
		glfwMakeContextCurrent(window);
		int res=gladLoadGL();
		if (!res) {
			fprintf(stderr, "Failed to init glad\n");
			getchar();
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