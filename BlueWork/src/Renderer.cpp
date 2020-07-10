#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "Engine.h"
#include "resource/ShapeLoader.h"
#include "scene/SceneManager.h"
#include "Utilities/Utilities.h"

Renderer::Renderer(): 
	Manager(nullptr), plane(nullptr), height(0), width(0){}

Renderer::Renderer(Program passShader, GLuint w, GLuint h):
	pass(passShader), width(w),height(h){
	passthrough = FrameBuffer(1920, 1080);
	passthrough.bind();
	passthrough.addTexture2D("passthrough", GL_RGBA, GL_RGBA, GL_COLOR_ATTACHMENT0);
	passthrough.addDepth();
	passthrough.drawBuffers();

	CameraBuffer = Camera::buildCamera();

	plane = ShapeLoader::MakeZPlane(1, 1);
}

void Renderer::SetDimensions(int h, int w){
	height = h;
	width = w;
}

void Renderer::cleanup() {
	lights.cleanup();
	//Manager->CleanUp();
	plane->cleanup();
	delete plane;
}

void updateShadowMatrices(glm::vec3 lightPosition, ShadowData& data) {
	float near_plane = .10f;
	data.FarPlane = 100.0f;
	glm::mat4 shadowP = glm::perspective(
		glm::radians(90.f), 
		(float)data.DepthPass.getWidth() / data.DepthPass.getHeight(), 
		near_plane, data.FarPlane);

	data.Projecttions[0] = shadowP * glm::lookAt(lightPosition,lightPosition + glm::vec3(1.f, 0.f, 0.f),glm::vec3(0.f,-1.f, 0.f));
	data.Projecttions[1] = shadowP * glm::lookAt(lightPosition,lightPosition + glm::vec3(-1.f, 0.f,0.f),glm::vec3(0.f,-1.f, 0.f));
	data.Projecttions[2] = shadowP * glm::lookAt(lightPosition,lightPosition + glm::vec3(0.f, 1.f, 0.f),glm::vec3(0.f, 0.f, 1.f));
	data.Projecttions[3] = shadowP * glm::lookAt(lightPosition,lightPosition + glm::vec3(0.f,-1.f, 0.f),glm::vec3(0.f, 0.f,-1.f));
	data.Projecttions[4] = shadowP * glm::lookAt(lightPosition,lightPosition + glm::vec3(0.f, 0.f, 1.f),glm::vec3(0.f,-1.f, 0.f));
	data.Projecttions[5] = shadowP * glm::lookAt(lightPosition,lightPosition + glm::vec3(0.f, 0.f,-1.f),glm::vec3(0.f,-1.f, 0.f));
}

void Renderer::collectLights() {
	int numberOfLights = Manager->GetNumberOfLights();

	for (int i = 0; i < numberOfLights; i++) {
		Light* light = Manager->GetLight(i);
		glm::mat4 finalTransform = light->ResolveFinalTransform();
		switch (light->lightType) {
		case LightType::Point:
			PointLight* pointLight = (PointLight*)light;
			unsigned int id = pointLight->GetId();
			//if (pointLight->CreatesShadow)
			{
				if (!MapContains(ShadowMap, id)) {
					ShadowMap.insert(std::make_pair(id, ShadowData()));
					ShadowMap[id].DepthPass = FrameBuffer(900, 900);
					ShadowMap[id].DepthPass.bind();
					ShadowMap[id].DepthPass.addTexture3D("depth", GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_DEPTH_ATTACHMENT);
					ShadowMap[id].DepthPass.drawBuffers();
					ShadowMap[id].DepthPass.check();
					ShadowMap[id].Projecttions.resize(6, glm::mat4(1));
					ShadowMap[id].DepthPassProgram = shadowProgram;
				}
				glm::vec3 position = pointLight->transform.FinalPosition();
				updateShadowMatrices(
					position,
					ShadowMap[id]);

				pointLight->data.FarPlane = ShadowMap[id].FarPlane;
				pointLight->data.shadow = true;
				pointLight->data.shadowId = std::distance(ShadowMap.begin(), ShadowMap.find(id));

				ShadowPass(id, position);
			}
			lightbuf.push_back(pointLight->pack());
		}
	}
}


void Renderer::ShadowPass(unsigned int id, glm::vec3 position) {
	int numberOfEntities = Manager->GetNumberOfEntities();
	ShadowMap[id].DepthPass.bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	for (int i = 0; i < numberOfEntities; i++) {
		Entity* entity = Manager->GetEntity(i);
		//if (!entity->CastShadow) continue;

		ShadowMap[id].DepthPassProgram.bind();
		glm::mat4 finalTransform = entity->ResolveFinalTransform();
		ShadowMap[id].DepthPassProgram.setUniform("model", &finalTransform);

		for (int i = 0; i < 6; i++)
			ShadowMap[id].DepthPassProgram.setUniform("shadowmats[" + std::to_string(i) + "]", &ShadowMap[id].Projecttions[i]);

		ShadowMap[id].DepthPassProgram.setUniform("far_plane", ShadowMap[id].FarPlane);
		ShadowMap[id].DepthPassProgram.setUniform("lightPos", &position);

		entity->geometry->draw();
	}
}

void Renderer::setup(SceneManager* manager) {
	Manager = manager;
	ShadowMap.clear();
}

void Renderer::updateLights() {
	lightbuf.clear();
	collectLights();
	if (lightbuf.size() > 0) {
		lights.bind();
		lights.setData<Light::PointData>(lightbuf, GL_DYNAMIC_DRAW);
		lights.unbind();
	}
}

void Renderer::pre() {
	Manager->UpdateCamera(height, width);
	updateLights();
}

void Renderer::render() {
	pre();

	passthrough.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	traverseGraph();

	post();
}

void Renderer::post() {
	FrameBuffer::bindDefualt();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, width, height);

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
			material->shader.setUniform("model", &finalTransform); 
			if (lightbuf.size() > 0) {
				material->shader.setUniform("numLights", lightbuf.size());
				lights.bind();
				lights.blockBinding(material->shader.getProgramID(), 1, "Lights");
			}
			camera->bindCamera(CameraBuffer.get(), material->shader);

			std::map<unsigned int, ShadowData>::iterator it;
			for (it = ShadowMap.begin(); it != ShadowMap.end(); it++) 
			{
				int idx = std::distance(ShadowMap.begin(), it);
				Texture t = it->second.DepthPass.getTexture("depth");
				int tid = t.activate(Engine::OPENTEXTURE + idx);
				material->shader.setUniform(std::string("depthmaps[") + std::to_string(idx)+"]",tid);
			}

			entity->geometry->draw(); // add some conditional stuff: castsshadow?
		}
	}
}
