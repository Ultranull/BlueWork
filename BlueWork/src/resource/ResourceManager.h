#pragma once

#include <vector>
#include <string>
#include <map>
#include <algorithm>

#include "AbstractLoader.h"
#include "Utilities/Utilities.h"

template<typename ResourceType>
class ResourceManager {
	std::string ResourcePath;

	std::map<std::string, ResourceType> Resources;
	std::vector<std::unique_ptr<AbstractLoader<ResourceType>>> Loaders;


public:

	ResourceManager() {

	}

	void SetPath(std::string path) {
		ResourcePath = path;
	}

	ResourceType Get(std::string name) {
		if (MapContains(Resources, name))
			return Resources[name];
		return ResourceType();
	}

	std::string GetTextureName(ResourceType r) {
		std::vector<std::string> values;
		if (Utilities::findByValue(values, Resources, r)) {
			return values[0];
		}
		return "";
	}

	bool LoadFile(std::string fileName, std::string defaultName, std::string metaData = "") {
		for (int i = 0; i < Loaders.size(); i++) {
			if (Loaders[i]->HasExtention(fileName.substr(fileName.find_last_of(".")))) {
				std::map<std::string, ResourceType> parsed = Loaders[i]->Parse(Loaders[i]->LoadFile(ResourcePath + fileName, defaultName, metaData));
				Resources.insert(parsed.begin(), parsed.end());
				return true;
			}
		}
		return false;
	}

	template<typename LoaderClass>
	void RegisterLoader() {
		Loaders.push_back(std::make_unique<LoaderClass>());
	}
};


/*

keep file type reading impl seperate

instance counts for resources
	-if level 1 loads file A
		- create one instance of A with a count of 1
	-if level 2 loads file A
		- increment count on A
	-if level 1 unloads
		- if count > 1? decrement A.count : unload A

	resource types:
		geometry
		shader
		material
		texture
		audio
		scene

	gltf2
		-can hold multiple resource types
	obj
		-obj + mtl
			-materials, images, geometries
	images
	shaders
		-identify type
	audio
	misc
		-user can define a resource type and make impl for it

*/


