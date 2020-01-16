#pragma once

#include <vector>
#include <string>

#include <glad/glad.h>

enum NodeType :GLuint{
	DUMMY=0,
	ENTITY,
	POINTLIGHT,
	DIRLIGHT,
	SPOTLIGHT

};

class Node{
	std::vector<Node*> children;
	Node *parent;

	NodeType type;
	std::string name;


public:
	Node();
	Node(NodeType t);

	void add(Node* child);
	void setParent(Node* p);
	NodeType getType();
	int getNumberOfChildren();
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

