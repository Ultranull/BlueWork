#pragma once

#include <glm/glm.hpp>

#include "graphics/Geometry.h"
#include "graphics/Material.h"
#include "scene/Node.h"


class GeometryComponent{
	Geometry* Mesh;
	Material MaterialData;
	Node* Owner;
	glm::mat4 Model;

public:

	GeometryComponent():
		Mesh(nullptr), Owner(nullptr), Model(1){

	}

	glm::mat4 GetModel() {
		if (Owner != nullptr) {
			return Owner->ResolveFinalTransform();
		}
		return Model;
	}

};

