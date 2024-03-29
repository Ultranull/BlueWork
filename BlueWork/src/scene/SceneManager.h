#pragma once

#include <vector>
#include <memory>

#include "Node.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"

#include "resource/Serializer.h"


class SceneManager{
	 template<typename T>
	 friend	nlohmann::json Serializer::GeneralCompose(T object);

	std::vector<std::unique_ptr<Node>> Nodes;
	std::vector<std::unique_ptr<Entity>> Entities;
	std::vector<std::unique_ptr<Light>> Lights;
	std::vector<std::unique_ptr<Camera>> Cameras;
	
	Camera* MainCamera;

	Node* Root;

public:

	SceneManager();

	Node* PushNode(Node* node);

	Node* GetRoot(void);

	Camera* GetMainCamera(void);
	void SetMainCamera(std::string name);
	void UpdateCamera(int height, int width);

	int GetNumberOfLights(void);
	Light* GetLight(int index);

	int GetNumberOfEntities(void);
	Entity* GetEntity(int index);

	void CleanUp(void);

};

