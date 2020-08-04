#pragma once

#include <map>
#include <string>
#include <stdio.h>
#include <memory>
#include <functional>

#include <glad/glad.h>

#include "graphics/ShaderProgram.h"
#include "graphics/Geometry.h"
#include "graphics/Material.h"

#include "Utilities/TaskQueue.h"

class SceneManager;

class Resource {
	std::map<std::string, Texture> textures;
	std::map<std::string, Shader> shaders;
	std::map<std::string, std::unique_ptr<Geometry>> geometries;

	std::string Manifest;

	TaskQueue<std::string> LoadTasks;
	std::function<void(void)> OnLoadSucess;

	Texture LoadGLTexture(const char *filename);
	Texture LoadGLsubTexture(const char *filename, int sub_x, int sub_y, int sub_width, int sub_height);

	void LoadAssetTask(std::string line);

public:
	std::string path = "assets/", texturePath = "textures/", shaderPath = "shaders/";
	Resource();

	bool ContainsName(std::string name);

	void QueueLoadTask(Task<std::string>* task);

	template<typename Class>
	void SetLoadSucessCallback(void(Class:: *loadCall)(void), Class* source) {
		OnLoadSucess = std::bind(loadCall, source);
	}
	void batchLoad(std::string manifest, bool queue = false);

	int LoadQueueSize();
	void ProcessNextLoadTask();

	std::string GetNextLoadString();

	void ImmediateLoadScene(std::string filename, SceneManager* scene);

	void LoadManifest(std::string fileName);

	void LoadScene(std::string fileName, SceneManager* manager);

	Texture addTexture(std::string name, const char *tex);
	void addTextures(std::string name, const char *tar, int sub_width, int sub_height, int ir, int ic);
	Texture getTexture(std::string name);
	GLint bindTexture(std::string name, GLuint sample);

	Shader addShader(std::string name);
	Shader getShader(std::string name);

	void addGeometry(std::string name,Geometry* geom);
	Geometry* getGeometry(std::string name);

	std::string GetGeometryName(Geometry* geom);
	std::string GetTextureName(Texture tex);
	std::string GetShaderName(Shader shader);


	void setPath(std::string texturePath, std::string shaderPath, std::string path);
	std::string getShaderPath(std::string file);

	std::string GetManifest();

	void cleanup();

	static Resource& getInstance() {
		static Resource instance;
		return instance;
	}
	Resource(Resource const&) = delete;
	void operator=(Resource const&) = delete;

};


#ifdef gg

#include <algorithm>

#include "Utilities/Utilities.h"


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


template<typename ResourceType>
class ResourceLibrary{
	std::map<std::string, ResourceType> Resources;

public:
	ResourceType Get(std::string name) {
		if(MapContains(Resources, name))
			return Resources[name];
		return ResourceType();
	}

};

template<typename ResourceType>
class AbstractLoader {
protected:
	std::vector<std::string> Extentions;

public:
	virtual ResourceType LoadFile(std::string fileName) = 0;
	virtual ResourceType Parse(std::string data) = 0;

	bool HasExtention(std::string ext) {
		return std::find(Extentions.begin(), Extentions.end(), ext) != Extentions.end();
	}
};

class ObjLoader: public AbstractLoader<Geometry>{};

template<typename ResourceType>
class ResourceManager {
	std::string ResourcePath;
	std::vector<AbstractLoader<ResourceType>> Loaders;


public:

	ResourceType LoadFile(std::string   fileName) {
		for (int i = 0; i < Loaders.size(); i++) {
			if (Loaders[i].HasExtention(fileName.substr(fileName.find_last_of(".")))) {
				return Loaders.LoadFile(ResourcePath  + fileName);
			}
		}
	}
};
#endif // gg
