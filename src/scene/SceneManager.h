#pragma once

#include <vector>
#include <memory>

#include "Node.h"
#include "Entity.h"
#include "Light.h"
#include "Camera.h"


class SceneManager{
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

	void SetManCamera(std::string name);

	int GetNumberOfLights(void);
	Light* GetLight(int index);

	int GetNumberOfEntities(void);
	Entity* GetEntity(int index);

	void CleanUp(void);

};

