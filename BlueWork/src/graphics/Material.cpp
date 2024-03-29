#include "Material.h"

#include "resource/Resource.h"
#include "resource/Serializer.h"
#include "Engine.h"

void Texture::load(){
	glTexParameteri(params.target, GL_TEXTURE_MAG_FILTER, params.FILTER);
	glTexParameteri(params.target, GL_TEXTURE_MIN_FILTER, params.FILTER);
	glTexParameteri(params.target, GL_TEXTURE_WRAP_S, params.WRAP);
	glTexParameteri(params.target, GL_TEXTURE_WRAP_T, params.WRAP);
	glTexParameteri(params.target, GL_TEXTURE_WRAP_R, params.WRAP);
	glGenerateMipmap(params.target);

}

void Texture::setSize(int w, int h){
	width = w;
	height = h;
}

void Texture::bind(){
	if (id == 0) {
		glGenTextures(1, &id);
	}
	glBindTexture(params.target, id);
}

GLint Texture::activate(GLenum texture) {
	glActiveTexture(texture);
	bind();
	//load();
	return texture - GL_TEXTURE0;
}

void Texture::cleanup(){
	glDeleteTextures(1, &id);
}

void Texture::unbind(GLuint target){
	glBindTexture(target, 0);
}

bool Texture::operator==(const Texture& rhs) {
	return rhs.id == id;
}

void Material::bind() {
	shader.bind();
	shader.setUniform("material.diffuse", diffuseMap.activate(Engine::DIFFUSE));
	shader.setUniform("material.specularMap", specularMap.activate(Engine::SPECULAR));
	shader.setUniform("material.normal", normalMap.activate(Engine::NORMALMAP));
	shader.setUniform("material.specular", &specular);
	shader.setUniform("material.color", &color);
	shader.setUniform("material.shininess", shininess);
}


void Material::cleanup() {
	shader.cleanup();
	diffuseMap.cleanup();
}
