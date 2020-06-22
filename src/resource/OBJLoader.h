#pragma once

#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

#include "../Engine.h"
#include "Resource.h"
#include "../graphics/VertexArray.h"
#include "graphics/Geometry.h"

class OBJLoader{

public:

	static Geometry* load(std::string fn);

	static std::vector<Engine::Vertex> parseOBJ(std::string fn, int& numberOfFaces);

};

