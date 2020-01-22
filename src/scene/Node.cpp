#include "Node.h"

Node::Node():
	children(), parent(nullptr), type("Node")
{}

Node::Node(std::string t):
	Node(){
	type = t;
}

void Node::add(Node* child) {
	children.push_back(child);
	child->setParent(this);
}
void Node::setParent(Node* p) {
	parent = p;
}

int Node::getNumberOfChildren(){
	return children.size();
}

std::string Node::GetType(){
	return type;
}

void Node::cleanup(){
	for (int i = 0; i < getNumberOfChildren(); i++)
		child(i)->cleanup();
}

Node* Node::child(GLuint index) {
	return children[index];
}

std::string Node::getName() {
	return name;
}

Node* Node::setName(std::string name) {
	this->name = name;
	return this;
}


