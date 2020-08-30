#pragma once

#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

#include "resource/MeshProcessing.h"
#include "resource/AbstractLoader.h"

#include "graphics/Geometry.h"

class ObjLoader : public AbstractLoader<std::shared_ptr<Geometry>> {


public:

	ObjLoader();

	std::map<std::string, std::shared_ptr<Geometry>> Parse(std::vector<unsigned char> data) override;
};

