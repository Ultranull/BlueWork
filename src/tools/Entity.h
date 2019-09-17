#pragma once
#include "Transform.h"
#include "Geometry.h"
#include "Material.h"
#include "Node.h"


struct Entity : public Node {
	static const unsigned int INSTANCED = 0b1;// maybe spec
	static const unsigned int INDEXED = 0b10;

	Transform transform;
	Geometry* geometry;
	Material material;

	unsigned int flags = 0;


	Entity() {}
	Entity(Geometry* geom, Material mat) :
		geometry(geom), material(mat), Node(NodeType::ENTITY) {}

	void cleanup();

};

struct Dummy : public Node {
	Transform transform;
	Dummy() :
		Node(NodeType::DUMMY) {}

	void cleanup();

};