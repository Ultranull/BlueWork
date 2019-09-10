#include "Node.h"



Node::Node()
{
}


Node::Node(NodeType t) :type(t), children(),parent(nullptr) {}

void Node::add(Node* child) {
	children.push_back(child);
	child->setParent(this);
}
void Node::setParent(Node* p) {
	parent = p;
}

NodeType Node::getType(){
	return type;
}

int Node::getNumberOfChildren(){
	return children.size();
}

void Node::cleanup(){
	for (int i = 0; i < getNumberOfChildren(); i++)
		child(i)->cleanup();
}

Node* Node::child(GLuint index) {
	return children[index];
}
