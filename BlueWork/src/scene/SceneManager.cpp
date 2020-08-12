#include "SceneManager.h"

#include <stack>
#include <sstream>

#include <loguru.hpp>


SceneManager::SceneManager():
	Root(nullptr), MainCamera(nullptr){
}

Node* SceneManager::PushNode(Node* node) {
	Node* out = nullptr;
	switch (node->GetType()){

		case NodeType::Node:
			Nodes.push_back(std::unique_ptr<Node>(node));
			out = Nodes.back().get();
			break;

		case NodeType::Entity:
			Entities.push_back(std::unique_ptr<Entity>((Entity*)node));
			out = Entities.back().get();
			break;

		case NodeType::Light:
			Lights.push_back(std::unique_ptr<Light>((Light*)node));
			out = Lights.back().get();
			break;

		case NodeType::Camera: {
			Cameras.push_back(std::unique_ptr<Camera>((Camera*)node));
			Camera* camera = Cameras.back().get();
			if (MainCamera == nullptr) {
				MainCamera = camera;
			}
			out = camera;
			break;
		}

		default:
			LOG_F(ERROR, "not a node %u", static_cast<int>(node->GetType()));
			break;
	}

	if (Root == nullptr && out != nullptr) {
		Root = out;
	}

	return out;
}

Node* SceneManager::GetRoot(void){
	if (Root == nullptr) {
		Root = PushNode(new Node());
		Root->setManager(this);
	}
	return Root;
}

Camera* SceneManager::GetMainCamera(void) {
	return MainCamera;
}

void SceneManager::SetMainCamera(std::string name) {
	Camera* camera = Root->findByName<Camera>(name);
	if (camera != nullptr) {
		MainCamera = camera;
	}
}

void SceneManager::UpdateCamera(int height, int width) {
	MainCamera->UpdateProjection(height, width);
	MainCamera->updateBuffer();
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
	LOG_F(INFO + 1, "Cleaning scene manager");
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


template<>
nlohmann::json Serializer::GeneralCompose(SceneManager* object) {	
	// holds the json pointer and parent node pointer
	typedef std::pair<nlohmann::json::json_pointer, Node*> NodeJsonPointer;
	typedef nlohmann::json::json_pointer jpointer; // easier to type

	Serializer& s = Serializer::getInstance();
	nlohmann::json json;


	std::stack<NodeJsonPointer> stack;
	stack.push(std::make_pair(jpointer("/Scene"), object->GetRoot()));

	NodeJsonPointer current;
	while (!stack.empty()) {
		current = stack.top();
		stack.pop();
		
		json[current.first] = s.Compose(current.second);

			jpointer childrenLocation = current.first;
			childrenLocation.push_back("children");
			json[childrenLocation] = nlohmann::json::array();

		for (int i = 0; i < current.second->getNumberOfChildren(); i++) {
			if (!current.second->child(i)->GetIsDependant()) {
				jpointer location = current.first;
				location.push_back("children");
				location.push_back(std::to_string(i));
				stack.push(std::make_pair(location, current.second->child(i)));
			}
		}
	}


	return json;
}