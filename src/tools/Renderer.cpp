#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Engine.h"


Renderer::Renderer(){}

Renderer::Renderer(Program passShader, GLuint w, GLuint h):
	pass(passShader), width(w),height(h){
	passthrough = FrameBuffer(1920, 1080);
	passthrough.addTexture2D("passthrough", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0);
	passthrough.addDepth();
	passthrough.drawBuffers();

	plane = Mesh({
		Vertex{{-1,-1,0},{},{0,0}},
		Vertex{{-1,1,0},{},{0,1}},
		Vertex{{1,1,0},{},{1,1}},
		Vertex{{1,1,0},{},{1,1}},
		Vertex{{1,-1,0},{},{1,0}},
		Vertex{{-1,-1,0},{},{0,0}},
		});
}

void Renderer::cleanup() {
	lights.cleanup();
	scene->cleanup();
}

void Renderer::collectLights(Node* root, glm::mat4 transform) {

	glm::mat4 parent = glm::mat4(1);
	switch (root->getType()) {
	case NodeType::DUMMY: {
		parent = (reinterpret_cast<Dummy*>(root))->transform.Model();
	}break;
	case NodeType::ENTITY: {
		parent = (reinterpret_cast<Entity*>(root))->transform.Model();
	}break;
	case NodeType::POINTLIGHT: {
		lights::Point light = reinterpret_cast<lights::Light*>(root)->pointLight;
		parent = glm::translate(glm::vec3(light.position));
		light.position = transform * light.position;
		lightbuf.push_back(light);
	}break;
	}

	for (int i = 0; i < root->getNumberOfChildren(); i++)
		collectLights(root->child(i), transform * parent);
}

void Renderer::setup(Node* root) {
	scene = root;
	lightbuf.clear();
	collectLights(root, glm::mat4(1));

	lights = UniformBuffer();
	lights.bind();
	lights.setData<lights::Point>(lightbuf, GL_DYNAMIC_DRAW);
	//lights.blockBinding(shader.getProgramID(), 1, "Lights");
	lights.unbind();
}

void Renderer::updateLights() {
	lightbuf.clear();
	collectLights(scene, glm::mat4(1));
	lights.bind();
	lights.setData<lights::Point>(lightbuf, GL_DYNAMIC_DRAW);
	lights.unbind();
}

void Renderer::pre() {}

void Renderer::render() {
	pre();

	passthrough.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	traverseGraph(scene, glm::mat4(1));

	post();
}

void Renderer::post() {
	FrameBuffer::bindDefualt();
	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pass.bind();
	pass.setUniform("model", &glm::mat4(1));
	pass.setUniform("passthrough", passthrough.getTexture("passthrough").activate(Engine::PASSMAP));
	plane.renderVertices(GL_TRIANGLES);
}

void Renderer::traverseGraph(Node* root, glm::mat4 transform) {

	glm::mat4 parent = glm::mat4(1);
	switch (root->getType()) {
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

			entity->geometry->drawArrays(); // add some conditional stuff: castsshadow?
		}

	}break;
	}


	for (int i = 0; i < root->getNumberOfChildren(); i++)
		traverseGraph(root->child(i), transform * parent);
}
