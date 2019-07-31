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

using namespace glm;
using namespace std;

Mesh loadOBJ(string fn);


struct Transform {
	vec3 position_m = vec3(0);
	vec3 scale_m = vec3(1);
	quat rotation_m;
	mat4 parent = mat4(1);

	mat4 Model() {
		return parent*T()*R()*S();
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
	VertexArray vaObject; // make it a pointer, pass around a mem loc
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

	void bind(Transform t) {
		shader.bind();
		shader.setUniform("material.diffuse", diffuseMap.activate(GL_TEXTURE0));
		shader.setUniform("material.specular", &specular);
		shader.setUniform("material.color", &color);
		shader.setUniform("material.shininess", shininess);
		shader.setUniform("model", &t.Model());
	}

};

struct Light {
	struct attunation {
		float quadratic=0;
		float linear=0;
		float constant=0;
		float padd = 0;
	};
	struct Point {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		glm::vec4 position;

	};
	struct Directional {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		glm::vec4 direction;
	};
};

struct Entity {
	static const unsigned int INSTANCED = 0b1;
	static const unsigned int INDEXED = 0b10;

	Transform transform;
	Geometry geometry;
	Material material;

	unsigned int params=0;

	Entity() {}
	Entity(Geometry geom, Material mat) :geometry(geom), material(mat) {}

	void render() {
		material.bind(transform);
		geometry.vaObject.bind();
		switch (params) {
		case Entity::INDEXED: {
			glDrawElements(geometry.topology, geometry.size, GL_UNSIGNED_INT, (void*)0);
		}break;
		case Entity::INSTANCED: {
			glDrawArraysInstanced(geometry.topology, 0, geometry.size, geometry.instances);
		}break;
		case (Entity::INDEXED | Entity::INSTANCED): {
			glDrawElementsInstanced(geometry.topology, geometry.size, GL_UNSIGNED_INT, (void*)0, geometry.instances);
		}break;
		default: {
			glDrawArrays(geometry.topology, 0, geometry.size);
		}
		}
		geometry.vaObject.unbind();
	}

};

/*
	
	lights and objects dont share mats

* */


class Game :public App {
	Resource *R;

	Camera cam;

	Mesh ground, monkey;
	Program uved;

	Entity monkey_e;

	UniformBuffer lights;

	void initGL() {
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glClearColor(.1, .1, .1, 1);
		glEnable(GL_TEXTURE_2D);
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

		ground = loadOBJ(R->path+"ground.obj");
		monkey = loadOBJ(R->path + "monkey.obj");

		Material uved_mat(vec4(1), vec4(1), 5.f);
		uved_mat.diffuseMap = R->getTexture("uvmap");
		uved_mat.shader = uved;

		Geometry monkey_geom;
		monkey_geom.size = monkey.vertices.size();
		monkey_geom.topology = GL_TRIANGLES;
		monkey_geom.vaObject = std::move(monkey.vaObject);

		monkey_e = Entity(monkey_geom, uved_mat);
		
		
		cam.perspective(window, 45, .1, 100);
		cam.bindCamera(uved);

		vector<Light::Point> lightbuf;
		lightbuf.push_back(Light::Point{
				vec4(.1,0,0,1),vec4(1,0,0,1),vec4(0,1,1,1),
				Light::attunation{1. / 2.,1,1},
				vec4(1,1,0,0)
			});
		lightbuf.push_back(Light::Point{
				vec4(0,0,.2,1),vec4(0,0,1,1),vec4(1,1,0,1),
				Light::attunation{1. / 2.,1,1},
				vec4(-1,-1,0,0)
			});
		lightbuf.push_back(Light::Point{
				vec4(.1),vec4(1),vec4(1),
				Light::attunation{1. / 2.,0,1},
				vec4(0,0,3,0)
			});

		lights = UniformBuffer();
		lights.bind();
		lights.setData<Light::Point>(lightbuf, GL_STATIC_DRAW);
		lights.blockBinding(uved.getProgramID(), 1, "Lights");
		lights.unbind();


	}

	void onClose() {
		R->cleanup(); 
		cam.cleanup();
	}

	void update(float delta) {
		cam.perspective(window, 45, .1, 100);
		cam.updateBuffer();
		glfwSetWindowTitle(window, to_string(fps).c_str());

		lights.bind();
		lights.setSubData(sizeof(Light::Point) * 0 + offsetof(Light::Point, position),
			sizeof(vec4), glm::value_ptr(vec4(cos(ticks)*2,sin(ticks) * 2,0,0)));
		lights.setSubData(sizeof(Light::Point) * 1 + offsetof(Light::Point, position),
			sizeof(vec4), glm::value_ptr(vec4(-cos(ticks) * 2,-sin(ticks) * 2,0,0)));
		lights.unbind();

	}

	void render(float delta) {

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		monkey_e.render();
		
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