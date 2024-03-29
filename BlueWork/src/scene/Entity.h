#pragma once
#include "Transform.h"
#include "graphics/Geometry.h"
#include "graphics/Material.h"
#include "Node.h"


struct Entity : public Node {

	std::shared_ptr<Geometry> geometry;
	Material material;

	bool CastShadow;

	union {
		unsigned char flags;
		//struct { should probably be in geometry
		//	unsigned Instanced : 1;
		//	unsigned Indexed : 1;
		//	unsigned res : 6;
		//};
	};


	Entity(): Entity("Entity",nullptr, Material()) {}

	Entity(std::shared_ptr<Geometry> geom, Material mat) :
		Entity("Entity", geom, mat) {}

	Entity(std::string typeName, std::shared_ptr<Geometry> geom, Material mat);

	void guidraw();

	void cleanup();

};