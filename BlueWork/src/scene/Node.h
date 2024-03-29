#pragma once

#include <vector>
#include <string>
#include <memory>

#include <glad/glad.h>

#include "Transform.h"

class SceneManager;

enum class NodeType {
	Node,
	Light,
	Entity,
	Camera
};

class Node{

	SceneManager* manager;

	NodeType type;

	static unsigned int NextId;

	bool IsDependant;

protected:
	std::vector<Node*> children;
	Node* parent;

	unsigned int Id;
	std::string TypeName;
	std::string name;

public:
	Transform transform;

	Node();
	Node(std::string,NodeType); 
	Node(unsigned int id, std::string typeName, NodeType t);

	glm::mat4 ResolveFinalTransform();

	void add(Node* child);
	void setParent(Node* p);
	int getNumberOfChildren();
	NodeType GetType();
	void cleanup();

	void debuggui();

	unsigned int GetParentId();
	unsigned int GetId();

	Node* child(GLuint index);

	std::string getName();
	Node* setName(std::string name);

	std::string GetTypeName();

	void SetDependancy(bool dependant);

	bool GetIsDependant();

	void setManager(SceneManager* sceneManager);

	template<class T>
	T* findByName(std::string name) {
		Node* current = this;
		if (current->getName().compare(name) == 0)
			return reinterpret_cast<T*>(current);

		for (int i = 0; i < getNumberOfChildren(); i++) {
			current = child(i);
			current = current->findByName<T>(name);
			if (current != nullptr)
				return (T*)(current);
		}
		return nullptr;
	}
};