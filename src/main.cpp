#include <fstream>
#include <sstream>

#include <string>

#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "tools/App.h"
#include "tools/Camera.h"
#include "tools/Mesh.h"
#include "tools/Resource.h"
#include "tools/ShaderProgram.h"
#include "tools/FrameBuffer.h"
#include "tools/VertexArray.h"

#include "tools/errorcheck.h"

#include "tools/Node.h"
#include "tools/Light.h"
#include "tools/Geometry.h"
#include "tools/Transform.h"
#include "tools/Entity.h"
#include "tools/Renderer.h"
#include "tools/OBJLoader.h"

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
	Resource *R;

	OBJLoader loader;

	Camera cam;

	Dummy scene;

	Renderer renderer;


	void initGL() {
		glEnable(GL_MULTISAMPLE); // move to a context class
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(.1, .1, .1, 1);
		//glEnable(GL_CULL_FACE);
	}
	
	void loading() {
	}

	void init() {
		R = &Resource::getInstance(); // maybe a good time to make that asset loader


		Shader uved_v = R->addShader("vertex.vert");
		Shader uved_f = R->addShader("fragment.frag");
		Shader pass_v = R->addShader("pass.vert");
		Shader pass_f = R->addShader("pass.frag");

		Program pass(pass_v, pass_f);
		viewportinit(window);
		renderer = Renderer(pass, width, height);

		R->addTexture("uvmap", "uvmap.bmp");

		Material uved_mat(vec4(1), vec4(.1), 5);
		uved_mat.diffuseMap = R->getTexture("uvmap");
		uved_mat.shader = Program(uved_v, uved_f);

		Entity* monkey = new Entity(loader.load(R->path + "monkey.obj"), uved_mat);
		monkey->setName("monkey");
		scene.add(monkey);
		monkey->add(new lights::Light(lights::Point{
			vec4(.1,0,0,1),vec4(1,0,0,1),vec4(1,0,0,1),
			lights::attunation{1. / 2.,1,1},
			vec4(1,1,0,0)
		}));
		scene.add(new lights::Light(lights::Point{
			vec4(.1),vec4(1),vec4(1),
			lights::attunation{1. / 2.,1,1},
			vec4(1,0,0,0)
		}));
		renderer.setup(&scene);

		cam.perspective(window, 45, .1, 100);
		cam.bindCamera(uved_mat.shader);


	}
	void onClose() {
		R->cleanup(); 
		cam.cleanup();
		renderer.cleanup();
	}

	void update(float delta) {
		cam.perspective(window, 45, .1, 100);
		cam.updateBuffer();
		renderer.updateLights();
		glfwSetWindowTitle(window, to_string(fps).c_str());

		scene.findByName<Entity>("monkey")->transform.rotate(radians(ticks * 30), vec3(0, 1, 0));
	}

	void render(float delta) {

		renderer.render();
	}

	void inputListener(float delta) {
		running = glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS;
		cam.orbit(window, delta, { 0,0,0 });
	}
public:

	Game(int width, int height, const char *title) :App(width, height, title) {

	}
};

int main() {
	Game game(700, 700, "");
	game.start();
	return 0;
}
