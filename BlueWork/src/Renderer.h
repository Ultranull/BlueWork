#pragma once

#include <vector>


#include "graphics/Buffer.h"
#include "scene/Node.h"
#include "scene/Light.h"
#include "graphics/ShaderProgram.h"
#include "scene/Entity.h"
#include "graphics/FrameBuffer.h"


/*
	steps:
		-resolve all transforms
		-collect list of each node type needed
		-bind lights
		-loop over entities and render


	Renderer class is given render elements and has its own implementations:

	steps:
		-resolve all transforms
		-collect list of each node type needed
		-bind lights
		-loop over entities and render
			-material class goes to renderer class
			-renderer class renders based on material class

	AbstractMaterial:
		-defines shader inputs
		-can have as many things as it needs
		-binds itself to a given shader program
		-library should have shared pointers so owners can have subclasses

	the only real difference here is the material
	baic:
		-color, specular, normal maps and variables 
		-simple phong + uvs

	PBR:
		-like basic but PBR stuff

	simple:
		-no lighting just uv or color
		
*/

struct ShadowData {
	FrameBuffer DepthPass;
	Program DepthPassProgram;
	float FarPlane;
	std::vector<glm::mat4> Projecttions;
};

struct Renderer {
	std::shared_ptr<UniformBuffer> CameraBuffer;

	UniformBuffer lights;
	std::vector<Light::PointData> lightbuf;
	std::map<unsigned int, ShadowData> ShadowMap;
	Program shadowProgram;


	FrameBuffer passthrough;
	std::shared_ptr<Geometry> plane;
	Program pass;

	SceneManager* Manager;

	GLuint width, height;

	Renderer();

	Renderer(Program passthrough, GLuint w, GLuint h);

	void SetDimensions(int h, int w);

	void cleanup();

	void collectLights(void);

	void setup(SceneManager* manager);

	void updateLights();

	void pre();

	void render();
	void traverseGraph();
	void ShadowPass(unsigned int id, glm::vec3 position);

	void post();
};