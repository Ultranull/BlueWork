#pragma once

#include "../Engine.h"
#include "Resource.h"
#include "../graphics/VertexArray.h"
#include "graphics/Geometry.h"

class ShapeLoader{
	Resource* R = &Resource::getInstance();

public:

	Geometry* MakePlane(unsigned int xSegments, unsigned int ySegments);
	Geometry* MakeZPlane(unsigned int xSegments, unsigned int zSegments);

	Geometry* load(std::vector<Engine::Vertex> vertices, std::vector<unsigned int> indices);
};

