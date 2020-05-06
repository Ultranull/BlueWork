#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Engine.h"
#include "resource/ShapeLoader.h"
#include "scene/SceneManager.h"

Renderer::Renderer(): 
	Manager(nullptr), plane(nullptr), height(0), width(0){}

Renderer::Renderer(Program passShader, GLuint w, GLuint h):
	pass(passShader), width(w),height(h){
	passthrough = FrameBuffer(1920, 1080);
	passthrough.addTexture2D("passthrough", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0);
	passthrough.addDepth();
	passthrough.drawBuffers();

	CameraBuffer = Camera::buildCamera();

	ShapeLoader loader;

	plane = loader.MakeZPlane(1, 1);
}

void Renderer::cleanup() {
	lights.cleanup();
	Manager->CleanUp();
	plane->cleanup();
	delete plane;
}

void Renderer::collectLights() {
	int numberOfLights = Manager->GetNumberOfLights();

	for (int i = 0; i < numberOfLights; i++) {
		Light* light = Manager->GetLight(i);
		glm::mat4 finalTransform = light->ResolveFinalTransform();
		switch (light->lightType) {
		case LightType::Point:
			PointLight* pointLight = (PointLight*)light;
			lightbuf.push_back(pointLight->pack());
		}
	}

}

void Renderer::setup(SceneManager* manager) {
	Manager = manager;
	lightbuf.clear();
	collectLights();

	lights = UniformBuffer();
	lights.bind();
	lights.setData<Light::PointData>(lightbuf, GL_DYNAMIC_DRAW);
	lights.unbind();
}

void Renderer::updateLights() {
	lightbuf.clear();
	collectLights();
	lights.bind();
	lights.setData<Light::PointData>(lightbuf, GL_DYNAMIC_DRAW);
	lights.unbind();
}

void Renderer::pre() {}

void Renderer::render() {
	pre();

	passthrough.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	traverseGraph();

	post();
}

void Renderer::post() {
	FrameBuffer::bindDefualt();
	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pass.bind();
	pass.setUniform("model", &glm::mat4(1));
	pass.setUniform("passthrough", passthrough.getTexture("passthrough").activate(Engine::PASSMAP));
	plane->draw();
}

void Renderer::traverseGraph() {
	int numberOfEntities = Manager->GetNumberOfEntities();

	for (int i = 0; i < numberOfEntities; i++) {
		Entity* entity = Manager->GetEntity(i);
		Camera* camera = Manager->GetMainCamera();
		Material* material = &entity->material;
		glm::mat4 finalTransform = entity->ResolveFinalTransform();
		if (!entity->flags) {
			material->bind();
			material->shader.setUniform("numLights", lightbuf.size());
			material->shader.setUniform("model", &finalTransform);
			lights.bind();
			lights.blockBinding(material->shader.getProgramID(), 1, "Lights");
			camera->bindCamera(CameraBuffer.get(), material->shader);

			entity->geometry->draw(); // add some conditional stuff: castsshadow?
		}
	}
}
