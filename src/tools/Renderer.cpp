#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

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

void Renderer::render() {
	render(scene, glm::mat4(1));
}

void Renderer::render(Node* root, glm::mat4 transform) {

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
		render(root->child(i), transform * parent);
}
