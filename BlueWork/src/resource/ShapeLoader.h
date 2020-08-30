#pragma once

#include <fstream>
#include <sstream>

#include <glm/glm.hpp>

#include <json.hpp>

#include "resource/MeshProcessing.h"
#include "resource/AbstractLoader.h"

#include "graphics/Geometry.h"

class ShapeLoader: public AbstractLoader<std::shared_ptr<Geometry>> {

public:

	ShapeLoader();

	std::vector<unsigned char> LoadFile(std::string fileName, std::string name = "", std::string metaData = "") override;

	std::map<std::string, std::shared_ptr<Geometry>> Parse(std::vector<unsigned char> data) override;

	Geometry* MakePlane(unsigned int xSegments, unsigned int ySegments);
	Geometry* MakeZPlane(unsigned int xSegments, unsigned int zSegments);
};

/* shape file format:
* 
* [
*	{
*		"type":"plane",
*		"xSegments": 5,
*		"ySegments": 5
*	},
*	{
*		"type": "uvsphere",
*		"parallels": some number idk,
*		"meridians":  some number idk
*	}
* ]
* 
* 
*/