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

#include "Node.h"

using namespace glm;
using namespace std;

Mesh loadOBJ(string fn);

struct {
	// spec defines
}Engine;

struct Transform{
	vec3 position_m = vec3(0);
	vec3 scale_m = vec3(1);
	quat rotation_m;

	mat4 Model() {
		return T()*R()*S();
	}

	mat4 T() {
		return translate(position_m);
	}
	mat4 S() {
		return scale(scale_m);
	}
	mat4 R() {
		return toMat4(rotation_m);
	}
};
struct Geometry {
	GLuint topology;
	int size,instances=1;
	VertexArray* vaObject; // make it a pointer, pass around a mem loc
	
	void drawArrays() {
		vaObject->bind();
		glDrawArrays(topology, 0, size);
		vaObject->unbind();
	}

	void cleanup() {
		vaObject->cleanup();
	}
};
struct Material {
	Texture diffuseMap;
	glm::vec4 color;
	glm::vec4 specular;
	float shininess;
	Program shader;

	Material():
		color(glm::vec4(.5)), specular(glm::vec4(.5)), 
		shininess(1){
	}
	Material(glm::vec4 col,glm::vec4 spec,float shine):
		color(col), specular(spec), shininess(shine) {
	}

	void bind() {
		shader.setUniform("material.diffuse", diffuseMap.activate(GL_TEXTURE0)); // add to spec: diffuse map always tex0
		shader.setUniform("material.specular", &specular);
		shader.setUniform("material.color", &color);
		shader.setUniform("material.shininess", shininess);
	}

	void cleanup() {
		shader.cleanup();
		diffuseMap.cleanup();
	}

};



struct Entity: public Node{
	static const unsigned int INSTANCED = 0b1;// maybe spec
	static const unsigned int INDEXED = 0b10;

	Transform transform;
	Geometry geometry;
	Material material;

	unsigned int flags=0;


	Entity() {}
	Entity(Geometry geom, Material mat) :
		geometry(geom), material(mat),Node(NodeType::ENTITY) {}

	void cleanup() {
		Node::cleanup();
		geometry.cleanup();
		material.cleanup();
	}

};

struct Dummy : public Node {
	Transform transform;
	Dummy():
		Node(NodeType::DUMMY){}

	void cleanup() {
		Node::cleanup();
	}

};
struct attunation {
	float quadratic = 0;
	float linear = 0;
	float constant = 0;
	float padd = 0;
};
struct Point{
	glm::vec4 ambient;
	glm::vec4 color;
	glm::vec4 specular;
	attunation att;
	glm::vec4 position;


};

struct Light : public Node {
	Point source;
	Light() :Light(Point{}) {}
	Light(Point p) :
		Node(NodeType::LIGHT),source(p) {}
	void cleanup() {
		Node::cleanup();
	}
};

struct Renderer {
	UniformBuffer lights;
	vector<Point> lightbuf;

	Node* scene;

	void cleanup() {
		lights.cleanup();
		scene->cleanup();
	}

	void collectLights(Node* root,mat4 transform) {

		mat4 parent = mat4(1);
		switch (root->getType()) {
		case NodeType::DUMMY: {
			parent = (reinterpret_cast<Dummy*>(root))->transform.Model();
		}break;
		case NodeType::ENTITY: {
			parent = (reinterpret_cast<Entity*>(root))->transform.Model();
		}break;
		case NodeType::LIGHT: {
			Point light = reinterpret_cast<Light*>(root)->source;
			parent = glm::translate(vec3(light.position));
			light.position = transform * light.position;
			lightbuf.push_back(light);
		}break;
		}

		for (int i = 0; i < root->getNumberOfChildren(); i++)
			collectLights(root->child(i), transform * parent);
	}

	void setup(Node* root) {
		scene = root;
		lightbuf.clear();
		collectLights(root, mat4(1));

		lights = UniformBuffer();
		lights.bind();
		lights.setData<Point>(lightbuf, GL_DYNAMIC_DRAW);
		//lights.blockBinding(shader.getProgramID(), 1, "Lights");
		lights.unbind();
		_getError();
	}

	void updateLights() {
		lightbuf.clear();
		collectLights(scene, mat4(1));
		lights.bind();
		lights.setData<Point>(lightbuf, GL_DYNAMIC_DRAW);
		lights.unbind();
	}

	void render() {
		render(scene, mat4(1));
	}
	void render(Node* root,mat4 transform) {

		mat4 parent = mat4(1);
		switch (root->getType()){
		case NodeType::DUMMY: {
			parent = reinterpret_cast<Dummy*>(root)->transform.Model();
		}break;
		case NodeType::ENTITY: {
			Entity* entity = reinterpret_cast<Entity*>(root);
			parent = entity->transform.Model();
			Program* shader = &entity->material.shader;
			if (!entity->flags) {
				shader->bind();
				entity->material.bind();
				shader->setUniform("numLights", lightbuf.size());
				shader->setUniform("model", &(transform * parent));
				lights.bind();
				lights.blockBinding(shader->getProgramID(), 1, "Lights");

				entity->geometry.drawArrays();
			}

		}break;
		}


		for (int i = 0; i < root->getNumberOfChildren(); i++)
			render(root->child(i), transform * parent);
	}
};

void dumpMesh(Mesh &mesh,string fn) {
	ofstream out(fn.c_str(), ios::binary);
	size_t size = mesh.vertices.size();
	out.write((char*)&size, sizeof(size));
	for (int i = 0; i < mesh.vertices.size(); i++) {
		Vertex v = mesh.vertices[i];
		out.write((char*)&v, sizeof(v));
	}
	out.close();
}

Mesh loadMesh(string fn) {
	vector<Vertex> verts;
	ifstream in(fn.c_str(), ios::binary); 
	if (!in) {
		string obj = fn;
		int dot = obj.find('.');
		obj.replace(obj.begin() + dot + 1, obj.end(), "obj");
		Mesh m = loadOBJ(obj);
		dumpMesh(m, fn);
		return m;
	}
	size_t size;
	in.read((char*)&size, sizeof(size));
	for (int i = 0; i < size; i++) {
		Vertex v;
		in.read((char*)&v, sizeof(v));
		verts.push_back(v);
	}
	in.close();
	return Mesh(verts);
}


class Game :public App {
	Resource *R;

	Camera cam;

	Mesh ground, monkey;
	Program uved;

	Entity* monkey_e;
	Light light,* light2;

	Dummy scene;

	UniformBuffer lights;

	Renderer renderer;

	void initGL() {
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(.1, .1, .1, 1);
		//glEnable(GL_CULL_FACE);
	}

	void loading() {
	}

	void init() {
		R = &Resource::getInstance();

		Shader uved_v = R->addShader("vertex.vert");
		Shader uved_f = R->addShader("fragment.frag");
		uved = Program(uved_v, uved_f);

		R->addTexture("uvmap", "uvmap.bmp");

		//ground = loadOBJ(R->path+"ground.obj");
		monkey = loadMesh(R->path + "monkey.mesh");


		Material uved_mat(vec4(1), vec4(1), 2);
		uved_mat.diffuseMap = R->getTexture("uvmap");
		uved_mat.shader = uved;

		Geometry monkey_geom;
		monkey_geom.size = monkey.vertices.size();
		monkey_geom.topology = GL_TRIANGLES;
		monkey_geom.vaObject = &monkey.vaObject;

		monkey_e = new Entity(monkey_geom, uved_mat);
		Entity* b = new Entity(monkey_geom, uved_mat);
		b->transform.position_m = vec3(3,0,0);
		b->transform.scale_m = vec3(.5);
		monkey_e->add(b);
		scene.add(monkey_e);

		light = Light(Point{
			vec4(.1,0,0,1),vec4(1,0,0,1),vec4(1,0,0,1),
			attunation{1. / 2.,1,1},
			vec4(1,1,0,0)
		});

		light2 = new Light(Point{
			vec4(.1),vec4(1),vec4(1),
			attunation{1. / 2.,1,1},
			vec4(1,0,0,0)
			});

		scene.add(&light);
		scene.add(light2);
		renderer.setup(&scene);

		cam.perspective(window, 45, .1, 100);
		cam.bindCamera(uved);

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

		//monkey_e->transform.rotation_m = angleAxis(radians(ticks*30), vec3(0, 1, 0));
		light2->source.position = vec4(cos(ticks)*2,sin(ticks)*2,0,0);
	}

	void render(float delta) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

Mesh loadOBJ(string fn) {

	vector<vec3> verts_;
	vector<vec3> norms_;
	vector<vec2> uv_;
	vector<Vertex> verts;

	ifstream in;
	in.open(fn.c_str(), ifstream::in);
	if (in.fail()) return Mesh();
	string line;
	while (!in.eof()) {
		getline(in, line);
		istringstream iss(line.c_str());
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			vec3 v;
			for (int i = 0; i < 3; i++) iss >> v[i];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 3, "vn ")) {
			iss >> trash >> trash;
			vec3 n;
			for (int i = 0; i < 3; i++) iss >> n[i];
			norms_.push_back(n);
		}
		else if (!line.compare(0, 3, "vt ")) {
			iss >> trash >> trash;
			vec2 uv;
			for (int i = 0; i < 2; i++) iss >> uv[i];
			uv_.push_back(vec2(uv.x, 1 - uv.y));
		}
		else if (!line.compare(0, 2, "f ")) {
			vector<ivec3> f;
			int vert, uv, norm;
			iss >> trash;
			while (iss >> vert >> trash >> uv >> trash >> norm) {
				vert--; uv--; norm--;
				verts.push_back(Vertex{ verts_[vert],{1,1,1} ,uv_[uv], norms_[norm] });
			}
		}
	}
	return Mesh(verts);
}