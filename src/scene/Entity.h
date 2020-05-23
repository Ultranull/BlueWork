#pragma once
#include "Transform.h"
#include "../Geometry.h"
#include "../Material.h"
#include "Node.h"


struct Entity : public Node {

	Geometry* geometry;
	Material material;

	union {
		unsigned char flags;
		struct {
			unsigned Instanced : 1;
			unsigned Indexed : 1;
			unsigned res : 6;
		};
	};


	Entity(): Entity("Entity",nullptr, Material()) {}

	Entity(Geometry* geom, Material mat) :
		Entity("Entity", geom, mat) {}

	Entity(std::string typeName, Geometry* geom, Material mat) :
		geometry(geom), material(mat), flags(0), Node(typeName, NodeType::Entity) {}

	void cleanup();

};