#pragma once

#include <map>
#include <string>
#include <stdio.h>
#include <memory>

#include <glad/glad.h>

#include"../graphics/ShaderProgram.h"
#include "graphics/Geometry.h"
#include "graphics/Material.h"

#define MapContains(MAP, KEY) MAP.find(KEY) != MAP.end()

class Resource {
	std::map<std::string, Texture> textures;

	std::map<std::string, Shader> shaders;

	std::map<std::string, std::unique_ptr<Geometry>> geometries;

	std::string Manifest;
public:
	std::string path = "assets/", texturePath = "textures/", shaderPath = "shaders/";
	Resource();

	bool ContainsName(std::string name);

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

	Texture LoadGLTexture(const char *filename);
	Texture LoadGLsubTexture(const char *filename, int sub_x, int sub_y, int sub_width, int sub_height);

	void setPath(std::string texturePath, std::string shaderPath, std::string path);
	std::string getShaderPath(std::string file);

	std::string GetManifest();

	void batchLoad(std::string manifest);

	void cleanup();

	static Resource& getInstance() {
		static Resource instance;
		return instance;
	}
	Resource(Resource const&) = delete;
	void operator=(Resource const&) = delete;

};
