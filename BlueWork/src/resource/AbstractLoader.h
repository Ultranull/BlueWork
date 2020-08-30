#pragma once

#include <vector>
#include <map>
#include <string>
#include <sstream>

#include <glm/glm.hpp>

#include "Engine.h"
#include "resource/MeshProcessing.h"
#include "Utilities/Utilities.h"

template<typename ResourceType>
class AbstractLoader {
protected:
	std::vector<std::string> Extentions;
	std::string DefaultName;
	std::string MetaData;
public:

	virtual std::vector<unsigned char> LoadFile(std::string fileName, std::string name="", std::string metaData="") {
		DefaultName = name;
		MetaData = metaData;
		return Utilities::readFileBytes(fileName.c_str());
	}
	virtual std::map<std::string, ResourceType> Parse(std::vector<unsigned char>) = 0;

	bool HasExtention(std::string ext) {
		return std::find(Extentions.begin(), Extentions.end(), ext) != Extentions.end();
	}
};
