#pragma once

#include <vector>

#include <glad/glad.h>

enum NodeType :GLuint{
	DUMMY=0,
	ENTITY,
	LIGHT,

};

class Node{
	std::vector<Node*> children;
	Node *parent;

	NodeType type;


public:
	Node();
	Node(NodeType t);

	void add(Node* child);
	void setParent(Node* p);
	NodeType getType();
	int getNumberOfChildren();
	void cleanup();

	Node* child(GLuint index);
};

