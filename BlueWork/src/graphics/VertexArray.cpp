#include "VertexArray.h"

#include <loguru.hpp>


VertexArray::VertexArray():
	buffers(){
	glGenVertexArrays(1, &id);
}

VertexArray::~VertexArray()
{
}

 void VertexArray::bind() {
	glBindVertexArray(id);
}

 void VertexArray::unbind() {
	glBindVertexArray(0);
}

 void VertexArray::cleanup() {
	 if (id != -1) {
		 LOG_F(INFO + 1, "Cleaning VAO");
		 std::map<std::string, std::unique_ptr<Buffer>>::iterator iter;
		 for (iter = buffers.begin(); iter != buffers.end(); iter++) {
			 Buffer* b = (*iter).second.get();
			 b->cleanup();
		 }
		 buffers.clear();
		 id = -1;
	 }
}

 void VertexArray::updateData(std::string name, GLintptr offset, GLsizeiptr size,const void * data) {
	Buffer *buf = buffers[name].get();
	bind();
	buf->bind();
	buf->setSubData(offset, size, data);
}

 Buffer * VertexArray::getBuffer(std::string name) {
	return buffers[name].get();
}

