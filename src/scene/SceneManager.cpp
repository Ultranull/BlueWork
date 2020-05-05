#include "SceneManager.h"

SceneManager::SceneManager():
	Root(nullptr), MainCamera(nullptr){
	Root = PushNode(new Node());
	Root->setManager(this);
}

Node* SceneManager::PushNode(Node* node) {
	switch (node->GetType()){

		case NodeType::Node:
			Nodes.push_back(std::unique_ptr<Node>(node));
			return Nodes.back().get();

		case NodeType::Entity:
			Entities.push_back(std::unique_ptr<Entity>((Entity*)node));
			return Entities.back().get();

		case NodeType::Light:
			Lights.push_back(std::unique_ptr<Light>((Light*)node));
			return Lights.back().get();

		case NodeType::Camera: {
			Cameras.push_back(std::unique_ptr<Camera>((Camera*)node));
			Camera* camera = Cameras.back().get();
			if (node->getName().compare("Main") == 0) {
				MainCamera = camera;
			}
			return camera;
		}

		default:
			printf("not a node %u", static_cast<int>(node->GetType()));
			break;
	}
	return nullptr;
}

Node* SceneManager::GetRoot(void){
	return Root;
}

void SceneManager::SetManCamera(std::string name) {
	Camera* camera = Root->findByName<Camera>(name);
	if (camera != nullptr) {
		MainCamera = camera;
	}
}

int SceneManager::GetNumberOfLights(void) {
	return Lights.size();
}

Light* SceneManager::GetLight(int index) {
	return Lights[index].get();
}

int SceneManager::GetNumberOfEntities(void) {
	return Entities.size();
}

Entity* SceneManager::GetEntity(int index) {
	return Entities[index].get();
}

void SceneManager::CleanUp(void) {
	for (size_t i = 0; i < Nodes.size(); i++) {
		Nodes[i]->cleanup();
	}
	for (size_t i = 0; i < Entities.size(); i++) {
		Entities[i]->cleanup();
	}
	for (size_t i = 0; i < Lights.size(); i++) {
		Lights[i]->cleanup();
	}
	for (size_t i = 0; i < Cameras.size(); i++) {
		Cameras[i]->cleanup();
	}
}