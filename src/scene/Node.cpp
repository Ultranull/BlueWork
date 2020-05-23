#include "Node.h"

#include "SceneManager.h"

unsigned int Node::NextId = 0;

Node::Node():
	Node("Node",NodeType::Node)
{}

Node::Node(std::string typeName, NodeType t):
	children(), parent(nullptr), type(t), Id(NextId++), TypeName(typeName)
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

unsigned int Node::GetParentId() {
	if (parent != nullptr) {
		return parent->GetId();
	}
	return -1;
}

unsigned int Node::GetId() {
	return Id;
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

std::string Node::GetTypeName() { return TypeName; }

void Node::setManager(SceneManager* sceneManager) {
	manager = sceneManager;
}

