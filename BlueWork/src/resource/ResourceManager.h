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


public:
	std::vector<std::unique_ptr<AbstractLoader<ResourceType>>> Loaders;

	ResourceManager() {

	}

	ResourceType Get(std::string name) {
		if(MapContains(Resources, name))
			return Resources[name];
		return ResourceType();
	}

	void LoadFile(std::string   fileName) {
		for (int i = 0; i < Loaders.size(); i++) {
			if (Loaders[i]->HasExtention(fileName.substr(fileName.find_last_of(".")))) {
				std::map<std::string, ResourceType> parsed = Loaders[i]->Parse(Loaders[i]->LoadFile(ResourcePath + fileName));
				Resources.insert(parsed.begin(), parsed.end());
				break;
			}
		}
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