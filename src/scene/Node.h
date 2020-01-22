#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>

#include "Transform.h"


class Node{

	std::string type;

protected:
	std::vector<Node*> children;
	Node *parent;


	std::string name;


public:
	Transform transform;

	Node();
	Node(std::string);

	void add(Node* child);
	void setParent(Node* p);
	int getNumberOfChildren();
	std::string GetType();
	void cleanup();

	Node* child(GLuint index);

	std::string getName();
	Node* setName(std::string name);

	template<class T>
	T* findByName(std::string name) {
		Node* current = this;
		if (current->getName().compare(name) == 0)
			return reinterpret_cast<T*>(current);

		for (int i = 0; i < getNumberOfChildren(); i++) {
			current = child(i);
			current = current->findByName<T>(name);
			if (current != nullptr)
				return reinterpret_cast<T*>(current);
		}
		return nullptr;
	}
};

