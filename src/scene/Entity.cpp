#include "Entity.h"

inline void Entity::cleanup() {
	Node::cleanup();
	geometry->cleanup();
	material.cleanup();
}
