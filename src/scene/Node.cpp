#include "Node.h"

#include "SceneManager.h"

Node::Node():
	Node(NodeType::Node)
{}

Node::Node(NodeType t):
	children(), parent(nullptr), type(t)
{}

glm::mat4 Node::ResolveFinalTransform() {
	glm::mat4 parentTransform = glm::mat4(1);
	bool shouldForce = false;
	if (parent != nullptr) {
		shouldForce = !parent->transform.IsValid();
		if (!transform.IsValid() || shouldForce) {
			parentTransform = parent->ResolveFinalTransform();
		}
	}
	return transform.FinalTransform(shouldForce, parentTransform);
}

void Node::add(Node* child) {
	Node* childPtr = manager->PushNode(child);
	children.push_back(childPtr);
	childPtr->setParent(this);
	childPtr->setManager(manager);
}
void Node::setParent(Node* p) {
	parent = p;
}

int Node::getNumberOfChildren(){
	return children.size();
}

NodeType Node::GetType(){
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

void Node::setManager(SceneManager* sceneManager) {
	manager = sceneManager;
}

