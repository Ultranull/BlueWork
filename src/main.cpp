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

#include "errorcheck.h"

#include "scene/Node.h"
#include "scene/Light.h"
#include "graphics/Geometry.h"
#include "scene/Transform.h"
#include "scene/Entity.h"
#include "Renderer.h"
#include "resource/ShapeLoader.h"
#include "scene/SceneManager.h"


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

const GLuint FORWARD = GLFW_KEY_W;//FIX! move to input class
const GLuint BACKWARD = GLFW_KEY_S;
const GLuint STRAFERIGHT = GLFW_KEY_D;
const GLuint STRAFELEFT = GLFW_KEY_A;
const GLuint JUMP = GLFW_KEY_SPACE;
const GLuint CROUCH = GLFW_KEY_LEFT_SHIFT;

class Player : public Entity {

	InputComponent* input;

	float speed;
	float mouseSpeed;
	double xpos, ypos;
	float hangle, vangle;

public:

	Player(Geometry* geometry, Material mat) :
		Entity(geometry, mat) {

		speed = 3.f;
		mouseSpeed = 0.005f;
		xpos = 0;
		ypos = 0;
		hangle = 0;
		vangle = 0;

		input = SystemManager::getInstance().CreateComponent<InputComponent>();
	}

	void OnMouseMove(double mouseX, double mouseY) {
		xpos = mouseX;
		ypos = mouseY;
	}

	void movement(float delta, int width, int height) {
		input->GetCursorPostion(xpos, ypos);
		input->SetCursorPostion(width / 2, height / 2);
		input->SetCursorMode(CursorInputMode::Disabled);

		hangle += mouseSpeed * float(width / 2 - xpos);//theta
		vangle += mouseSpeed * float(height / 2 - ypos);//phi

		constexpr float piHalves = glm::half_pi<float>();
		
		vec3 direction = vec3(cos(vangle) * sin(hangle),
			sin(vangle),
			cos(vangle) * cos(hangle));
		vec3 right(sin(hangle - piHalves),
			0,
			cos(hangle - piHalves));
		vec3 up = cross(right, direction);
		vec3 front = cross(vec3(0, 1, 0), right);

		vec3 xaxis = cross(up, direction);
		xaxis = normalize(xaxis);

		mat3 rotation = {
			xaxis.x, xaxis.y, xaxis.z,
			up.x, up.y, up.z,
			direction.x, direction.y, direction.z,
		};

		transform.Rotation() = quat_cast(rotation);

		if (input->GetKeyState(FORWARD) == InputState::Pressed) {
			transform.Position() += front * delta * speed;
		}
		if (input->GetKeyState(BACKWARD) == InputState::Pressed) {
			transform.Position() -= front * delta * speed;
		}
		if (input->GetKeyState(STRAFERIGHT) == InputState::Pressed) {
			transform.Position() += right * delta * speed;
		}
		if (input->GetKeyState(STRAFELEFT) == InputState::Pressed) {
			transform.Position() -= right * delta * speed;
		}
		if (input->GetKeyState(JUMP) == InputState::Pressed) {
			transform.Position() += vec3(0, 1, 0) * delta * speed;
		}
		if (input->GetKeyState(CROUCH) == InputState::Pressed) {
			transform.Position() -= vec3(0, 1, 0) * delta * speed;
		}
	}
};

class Game :public App {
	SystemManager& systemManager;
	Resource *R;

	SceneManager level1;

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
	
	void loading() {
	}

	void init() {
		R = &Resource::getInstance(); // maybe a good time to make that asset loader

		Serializer::getInstance().CommonParsers();

		systemManager.RegisterSystem(InputSystem::Name, new InputSystem(window));

		R->batchLoad(
			"uvmap.bmp:uvmap;"
			"vertex.vert:;"
			"fragment.frag:;"
			"pass.vert:;"
			"pass.frag:;"
			"monkey.obj:monkey;"
			"knot.obj:knot;"
		);

		R->addGeometry("ground", ShapeLoader().MakePlane(10, 10));

		Shader uved_v = R->getShader("vertex.vert");
		Shader uved_f = R->getShader("fragment.frag");
		Shader pass_v = R->getShader("pass.vert");
		Shader pass_f = R->getShader("pass.frag");

		Program pass(pass_v, pass_f);

		renderer = Renderer(pass, width, height);

		Material uved_mat(vec4(1), vec4(.5), 25);
		uved_mat.diffuseMap = R->getTexture("uvmap");
		uved_mat.shader = Program(uved_v, uved_f);

		Entity* monkey = new Entity(R->getGeometry("monkey"), uved_mat);
		monkey->setName("monkey");

		Entity* monkey2 = new Entity(R->getGeometry("monkey"), uved_mat);
		monkey2->setName("monkey2");
		monkey2->transform.translate(vec3(2, 2, 0));

		Entity* ground = new Entity(R->getGeometry("ground"), uved_mat);
		ground->transform.translate(vec3(0, -1, 0));
		ground->transform.scale(vec3(10));

		Entity* ball = new Entity(R->getGeometry("knot"), uved_mat);

		Node* scene = level1.GetRoot();

		Camera* mainCamera = new Camera();
		mainCamera->setName("camera 1");
		mainCamera->transform.translate(vec3(0, 3, -5));
		mainCamera->transform.rotate(glm::radians(30.f), glm::vec3(1, 0, 0));

		Camera* camera2 = new Camera();
		camera2->setName("camera 2");
		camera2->transform.translate(vec3(0, 0, 6));
		camera2->transform.rotate(glm::radians(180.f), glm::vec3(0, 1, 0));

		Camera* camera3 = new Camera();
		camera3->setName("camera 3");
		camera3->UpdateProjection(height, width, CameraSettings{
				.mode = CameraSettings::Mode::Orthographic,
				.OrthographicData = {
					.NearPlane = 1.,
					.FarPlane = 100.,
					.Left = -4.,
					.Right = 4.,
					.Bottom = -4.,
					.Top = 4.
				}
			});
		camera3->transform.translate(vec3(0, 5, 5));
		camera3->transform.rotate(glm::radians(180.), glm::vec3(0, 1, 0));
		camera3->transform.rotateBy(glm::angleAxis(glm::radians(-45.f), glm::vec3(1, 0, 0)));

		player = new Player(R->getGeometry("monkey"), uved_mat);

		scene->add(player);
		player->add(mainCamera);

		scene->add(camera3);
		camera3->add(ball);

		scene->add(ground);

		scene->add(monkey);
		monkey->add(monkey2);
		monkey2->add(camera2);

		level1.SetMainCamera("camera 1");

		PointLight* whiteLight = new PointLight(Light::PointData{
			vec4(1),vec4(1),vec4(1),
			Light::attunation{1. / 3.,1,1},
			0,
			vec4(0)
		});
		whiteLight->transform.translate(vec3(0, 0, 3));

		PointLight* redLight = new PointLight(Light::PointData{
			vec4(.5),vec4(1.,0.,0.,1.),vec4(1),
			Light::attunation{1. / 3.,1,1},
			0,
			vec4(0)
		});
		redLight->transform.translate(vec3(3, 0, 0));

		scene->add(whiteLight);
		monkey2->add(redLight);

		renderer.setup(&level1);


		LOG_F(INFO, "%s", Serializer::getInstance().GeneralCompose(&level1).dump().c_str());

		systemManager.start();
	}
	void onClose() {
		systemManager.CleanUp();
		R->cleanup(); 
		renderer.cleanup();
	}

	void update(float delta) {

		glfwSetWindowTitle(window, to_string(fps).c_str());

		level1.GetRoot()->findByName<Entity>("monkey")->transform.rotate(radians(ticks * 30), vec3(0, 1, 0));
		level1.GetRoot()->findByName<Entity>("monkey2")->transform.rotate(radians(ticks * 45 ), vec3(0, 1, 0));

		level1.UpdateCamera(height, width);
		renderer.updateLights();
	}

	void render(float delta) {

		renderer.SetDimensions(height, width);
		renderer.render();
	}

	void inputListener(float delta) {
		running = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;

		player->movement(delta, width, height);

		if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
			level1.SetMainCamera("camera 1");
		}

		if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
			level1.SetMainCamera("camera 2");
		}

		if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
			level1.SetMainCamera("camera 3");
		}
	}
public:

	Game(int width, int height, const char *title) :
		App(width, height, title), systemManager(SystemManager::getInstance()) {

	}
};


int main(int argc, char** argv) {
	loguru::g_preamble_thread = false;
	loguru::g_preamble_uptime = false;

	loguru::init(argc, argv);
	loguru::add_file("verbose_log.log", loguru::Truncate, loguru::Verbosity_MAX);
	loguru::add_file("log.log", loguru::Truncate, loguru::Verbosity_INFO);

	Game game(700, 700, "");
	game.start();
	return 0;
}
