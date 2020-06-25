#pragma once

#include "../Engine.h"
#include "Resource.h"
#include "../graphics/VertexArray.h"
#include "graphics/Geometry.h"

class ShapeLoader{

public:

	static Geometry* MakePlane(unsigned int xSegments, unsigned int ySegments);
	static Geometry* MakeZPlane(unsigned int xSegments, unsigned int zSegments);

	static Geometry* load(std::vector<Engine::Vertex> vertices, std::vector<unsigned int> indices);
};

