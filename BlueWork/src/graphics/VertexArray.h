#pragma once

#include <glad/glad.h>
#include <map>
#include <string>
#include <memory>

#include "Buffer.h"

class VertexArray{
	GLuint id;
	std::map<std::string, std::unique_ptr<Buffer>> buffers;
public:
	VertexArray();
	~VertexArray();

	void bind();
	void unbind();

	void cleanup();

	template<typename T> 
	Buffer* bindBuffer(std::string name, GLenum type);

	void updateData(std::string name, GLintptr offset, GLsizeiptr size,const void *data);

	Buffer* getBuffer(std::string name);

};
template<typename T>
inline Buffer * VertexArray::bindBuffer(std::string name, GLenum type) {
	bind();
	Buffer *buf = new Buffer(type);
	buffers.insert(std::make_pair(name, std::unique_ptr<Buffer>(buf)));
	buf->bind();
	return buf;
}
