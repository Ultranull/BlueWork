#pragma once

#include<glad/glad.h>

#include <glm/glm.hpp>

#include "graphics/ShaderProgram.h"
#include "scene/Transform.h"


struct TextureTraits {
	GLuint target;
	GLuint FILTER;
	GLuint WRAP;
	GLuint internalformat, format;
};

static TextureTraits RGBA_2D = {GL_TEXTURE_2D,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_RGBA,GL_RGBA};
static TextureTraits RGB_2D = {GL_TEXTURE_2D,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_RGB,GL_RGB};
static TextureTraits DEPTH_3D = { GL_TEXTURE_CUBE_MAP,GL_NEAREST,GL_CLAMP_TO_EDGE,GL_DEPTH_COMPONENT,GL_DEPTH_COMPONENT };

struct Texture {

	GLuint id;
	TextureTraits params;

	int width;
	int height;
	Texture():Texture(RGBA_2D) {}

	Texture(TextureTraits t):
		params(t), width(0), height(0), id(0){
		glGenTextures(1, &id);
	}

	void load();
	void setSize(int w,int h);

	void bind();
	GLint activate(GLenum texture);
	void cleanup();

	static void unbind(GLuint target = GL_TEXTURE_2D);
};

struct Material { // needs to handle more stuff
	Texture diffuseMap;
	glm::vec4 color;
	glm::vec4 specular;
	float shininess;
	Program shader;

	Material() :
		color(glm::vec4(.5)), specular(glm::vec4(.5)),
		shininess(1) {
	}
	Material(glm::vec4 col, glm::vec4 spec, float shine) :
		color(col), specular(spec), shininess(shine) {
	}

	void bind();

	void cleanup();

};

