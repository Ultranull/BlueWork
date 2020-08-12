#ifdef  LEAK_DETECT
#include <vld.h>
#endif //  LEAK_DETECT


#include <fstream>
#include <sstream>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <loguru.hpp>

#include "App.h"
#include "scene/Camera.h"
#include "resource/Resource.h"
#include "resource/Serializer.h"
#include "graphics/ShaderProgram.h"
#include "graphics/FrameBuffer.h"
#include "graphics/VertexArray.h"

#include "componentSystem/SystemManager.h"

#include "components/input system/InputComponent.h"
#include "components/input system/InputSystem.h"

#include "Utilities/errorcheck.h"

#include "scene/Node.h"
#include "scene/Light.h"
#include "graphics/Geometry.h"
#include "scene/Transform.h"
#include "scene/Entity.h"
#include "Renderer.h"
#include "resource/ShapeLoader.h"
#include "scene/SceneManager.h"

#include "Player.h"
#include "Lamp.h"

#include "Utilities/DebugGui.h"

using namespace glm;
using namespace std;


//void dumpMesh(Mesh &mesh,string fn) {
//	ofstream out(fn.c_str(), ios::binary);
//	size_t size = mesh.vertices.size();
//	out.write((char*)&size, sizeof(size));
//	for (int i = 0; i < mesh.vertices.size(); i++) {
//		Vertex v = mesh.vertices[i];
//		out.write((char*)&v, sizeof(v));
//	}
//	out.close();
//}
//
//Mesh loadMesh(string fn) {
//	vector<Vertex> verts;
//	ifstream in(fn.c_str(), ios::binary); 
//	if (!in) {
//		string obj = fn;
//		int dot = obj.find('.');
//		obj.replace(obj.begin() + dot + 1, obj.end(), "obj");
//		Mesh m = loadOBJ(obj);
//		dumpMesh(m, fn);
//		return m;
//	}
//	size_t size;
//	in.read((char*)&size, sizeof(size));
//	for (int i = 0; i < size; i++) {
//		Vertex v;
//		in.read((char*)&v, sizeof(v));
//		verts.push_back(v);
//	}
//	in.close();
//	return Mesh(verts);
//}



class Game :public App {
	SystemManager& systemManager;
	Resource& R;

	SceneManager level1, level2, loadingScreen;

	Renderer renderer;

	Player* player;


	void initGL() {
		glEnable(GL_MULTISAMPLE); // move to a context class
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(.1f, .1f, .1f, 1.f);
		glEnable(GL_CULL_FACE);
	}

	void init() {

		DebugGui::Initialize(window);
		DebugGui::PushDraw(&Game::debugGui, this);

		Serializer::getInstance().Initialize();
		Player::RegisterSerializer();
		Lamp::RegisterSerializer();

		systemManager.RegisterSystem(InputSystem::Name, new InputSystem(window));

		R.batchLoad("pass.vert:;"
					"pass.frag:;"
					"depthPoint.vert:;"
					"depthPoint.geom:;"
					"depthPoint.frag:;");

		R.addGeometry("xy-plane", ShapeLoader::MakeZPlane(1, 1));

		R.ImmediateLoadScene("loadingScreen.scene", &loadingScreen);

		renderer = Renderer(
			Program(R.getShader("pass.vert"), R.getShader("pass.frag")), 
			width, height);

		renderer.shadowProgram =
			Program(R.getShader("depthPoint.vert"), R.getShader("depthPoint.frag"), R.getShader("depthPoint.geom"));
		renderer.setup(&loadingScreen);

		R.LoadManifest("test.scene");
		R.LoadManifest("test2.scene");

		Task<std::string>* task = new Task<std::string>();
		task->Data = "test.scene";
		task->Notify = [&](std::string data) {
			R.LoadScene(data, &level1);
		};
		R.QueueLoadTask(task);

		Task<std::string>* task2 = new Task<std::string>();
		task2->Data = "test2.scene";
		task2->Notify = [&](std::string data) {
			R.LoadScene(data, &level2);
		};
		R.QueueLoadTask(task2);

		R.SetLoadSucessCallback(&Game::OnLoadSucess, this);
		

		//Serializer::getInstance().SaveFile("loadingScreen", &loadingScreen);

		systemManager.start();
	}

	bool loaded = false;
	void OnLoadSucess() {
		renderer.setup(&level1);
		player = level1.GetRoot()->findByName<Player>("player");
		loaded = true; 
		Serializer::getInstance().SaveFile("level1", &level1);
	}

	virtual void onClose() {
		systemManager.CleanUp();
		R.cleanup(); 
		renderer.cleanup();
		level1.CleanUp();
		level2.CleanUp();
		loadingScreen.CleanUp();    
		
		DebugGui::CleanUp();
	}

	void update(float delta) {

		if (loaded) { // temporary
			level1.GetRoot()->findByName<Entity>("barrel")->transform.rotateBy(0.01, vec3(0, 1, 0));
		}
		else
		{
			R.ProcessNextLoadTask();
		}

	}

	void debugGui() {
		if (!loaded) {
			ImGuiIO& io = ImGui::GetIO();
			std::string next = R.GetNextLoadString();
			ImGuiWindowFlags window_flags = 0;
			window_flags |= ImGuiWindowFlags_NoTitleBar;
			window_flags |= ImGuiWindowFlags_NoScrollbar;
			window_flags |= ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoResize;
			window_flags |= ImGuiWindowFlags_NoCollapse;
			window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
			ImGui::SetNextWindowSize(ImVec2(300, 50));
			ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
			ImGui::Begin("Test Window.", nullptr, window_flags);
			ImGui::Text("Loading %s", next.c_str());
			ImGui::End();
		}

		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_NoScrollbar;
		window_flags |= ImGuiWindowFlags_NoMove;
		window_flags |= ImGuiWindowFlags_NoResize;
		window_flags |= ImGuiWindowFlags_NoCollapse;
		window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
		ImGui::SetNextWindowBgAlpha(0.5f);
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("w1", nullptr, window_flags);
		ImGui::Text(
			"Application average %.3f ms/frame (%.1f FPS, %i engine FPS)",
			1000.0f / ImGui::GetIO().Framerate,
			ImGui::GetIO().Framerate,
			fps);
		ImGui::End();
	}

	void render(float delta) {
		renderer.SetDimensions(height, width);
		renderer.render();

		DebugGui::Render();
	}

	void inputListener(float delta) {
		running = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;

		if (loaded) {// temporary
			player->movement(delta, width, height);

			if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
				level1.SetMainCamera("Main");
			}

			if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
				player = level2.GetRoot()->findByName<Player>("player");
				renderer.setup(&level2);
			}
			if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
				player = level1.GetRoot()->findByName<Player>("player");
				renderer.setup(&level1);
			}
		}
	}
public:

	Game(int width, int height, const char *title) :
		App(width, height, title), 
		systemManager(SystemManager::getInstance()),
		R(Resource::getInstance()){

	}
};

int main(int argc, char** argv) {
	loguru::g_preamble_thread = false;
	loguru::g_preamble_uptime = false;

	loguru::init(argc, argv);
	loguru::add_file("verbose_log.log", loguru::Truncate, loguru::Verbosity_MAX);
	loguru::add_file("log.log", loguru::Truncate, loguru::Verbosity_INFO);

	Game* game = new Game(700, 700, "Game");
	game->start();
	delete game;
	return 0;
}
