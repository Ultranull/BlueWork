#include "Entity.h"

void Entity::cleanup() {
	Node::cleanup();
	geometry->cleanup();
	material.cleanup();
}
