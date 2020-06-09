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
		Serializer::getInstance().Initialize();
		Player::RegisterSerializer();

		systemManager.RegisterSystem(InputSystem::Name, new InputSystem(window));

		R.batchLoad("test.png:loading;"
					"loadingScreen.vert:;"
					"loadingScreen.frag:;"
					"pass.vert:;"
					"pass.frag:;");


		ShapeLoader loader;
		R.addGeometry("xy-plane", loader.MakeZPlane(1, 1));

		Node* lsroot = loadingScreen.GetRoot();

		Material lsmat(vec4(1), vec4(0), 0);
		lsmat.diffuseMap = R.getTexture("loading");
		lsmat.shader = Program(R.getShader("loadingScreen.vert"), R.getShader("loadingScreen.frag"));

		lsroot->add(new Entity(R.getGeometry("xy-plane"), lsmat));
		lsroot->add(new Camera(CameraSettings{
			.mode = CameraSettings::Mode::Orthographic,
			.OrthographicData = {
				.NearPlane = .01,
				.FarPlane = 1,
				.Left = -1,
				.Right = 1,
				.Bottom = -1,
				.Top = 1
			}
		}));

		renderer = Renderer(
			Program(R.getShader("pass.vert"), R.getShader("pass.frag")), 
			width, height);
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

		R.addGeometry("ground", ShapeLoader().MakePlane(10, 10));

		R.SetLoadSucessCallback(bind(&Game::OnLoadSucess, this));
		

		//Serializer::getInstance().SaveFile("test", &level1);

		systemManager.start();
	}

	bool loaded = false;
	void OnLoadSucess() {
		renderer.setup(&level1);
		player = level1.GetRoot()->findByName<Player>("player");
		loaded = true;
	}

	virtual void onClose() {
		systemManager.CleanUp();
		R.cleanup(); 
		renderer.cleanup();
		level1.CleanUp();
		level2.CleanUp();
	}

	void update(float delta) {

		glfwSetWindowTitle(window, to_string(fps).c_str());

		if (loaded) { // temporary
			level1.GetRoot()->findByName<Entity>("monkey")->transform.rotate(radians(ticks * 30), vec3(0, 1, 0));
			level1.GetRoot()->findByName<Entity>("monkey2")->transform.rotate(radians(ticks * 45), vec3(0, 1, 0));
		}
		else
		{
			R.ProcessNextLoadTask();
		}
		renderer.updateLights();

	}

	void render(float delta) {
		renderer.SetDimensions(height, width);
		renderer.render();
	}

	void inputListener(float delta) {
		running = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;

		if (loaded) {// temporary
			player->movement(delta, width, height);

			if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
				level1.SetMainCamera("Main");
			}

			if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
				level1.SetMainCamera("camera 2");
			}

			if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
				level1.SetMainCamera("camera 3");
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

	Game* game = new Game(700, 700, "");
	game->start();
	delete game;
	return 0;
}
