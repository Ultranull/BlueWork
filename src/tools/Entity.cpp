#include "Entity.h"

inline void Entity::cleanup() {
	Node::cleanup();
	geometry->cleanup();
	material.cleanup();
}

inline void Dummy::cleanup() {
	Node::cleanup();
}
