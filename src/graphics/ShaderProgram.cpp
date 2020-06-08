#include "ShaderProgram.h"
#include <vector>
#include <string>

#include <loguru.hpp>

#include "resource/Serializer.h"
#include "resource/Resource.h"
#include "Utilities/Utilities.h"

using namespace std;



bool compile(const char* file, GLuint id) {
	GLint result = GL_FALSE;
	int infoLogLength;
	LOG_F(INFO+1, "Compiling shader: %s (%u)", file, id);
	string content = Utilities::readFile(file);
	if (content.compare("") == 0) {
		LOG_F(ERROR, "compile error: file not found!");
		return false; 
	}
	char const * src = content.c_str();
	const GLint length = content.size();
	glShaderSource(id, 1, &src, &length);
	glCompileShader(id);

	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetShaderInfoLog(id, infoLogLength, NULL, &errormessage[0]);
		LOG_F(WARNING, "%s compile error:\n\t%s", file, &errormessage[0]);
		return false;
	}
	return true;
}

Shader::Shader(std::string file, GLuint t) :
	type(t), FileName(file) {
	id = glCreateShader(t);
	if (!compile(file.c_str(), id)) {
		exit(-1);
	}
}

void Shader::cleanup(){
	glDeleteShader(id);
}

Program::Program(Shader vert, Shader frag):vertex(vert),fragment(frag) {

	GLint result = GL_FALSE;
	int infoLogLength;

	if (vert.id == -1 || frag.id == -1) {
		LOG_F(FATAL, "Cannot link invalid shaders!");
		exit(-1);
	}

	LOG_F(INFO+1, "linking program: %d %d", vertex.id, fragment.id);
	programID = glCreateProgram();
	glAttachShader(programID, vertex.id);
	glAttachShader(programID, fragment.id);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		LOG_F(FATAL, "link error:\n%s", &errormessage[0]);
		exit(-1);
	}
	glDetachShader(programID, vertex.id);
	glDetachShader(programID, fragment.id);
}
Program::Program(Shader vert, Shader frag, Shader geom) :vertex(vert), fragment(frag),geometry(geom) {

	GLint result = GL_FALSE;
	int infoLogLength;

	LOG_F(INFO+1, "linking program");
	programID = glCreateProgram();
	glAttachShader(programID, vertex.id);
	glAttachShader(programID, fragment.id);
	glAttachShader(programID, geometry.id);
	glLinkProgram(programID);

	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 1) {
		vector<char> errormessage(infoLogLength + 1);
		glGetProgramInfoLog(programID, infoLogLength, NULL, &errormessage[0]);
		LOG_F(FATAL, "link error:\n%s\n", &errormessage[0]);
		exit(-1);
	}
}

Program::~Program() {
}

void Program::cleanup() {
	vertex.cleanup();
	fragment.cleanup();
	geometry.cleanup();
	glDeleteProgram(programID);
}

void Program::setUniform(std::string name, glm::vec4 *v) {
	GLuint id = glGetUniformLocation(programID, name.c_str());
	if (id == -1)return;
	glUniform4fv(id, 1, &(*v)[0]);
}

void Program::setUniform(std::string name, glm::vec3 *v) {
	GLuint id = glGetUniformLocation(programID, name.c_str());
	if (id == -1)return;
	glUniform3fv(id,1, &(*v)[0]);
}

void Program::setUniform(std::string name, glm::vec2 *v) {
	GLuint id = glGetUniformLocation(programID, name.c_str());
	if (id == -1)return;
	glUniform2fv(id, 1, &(*v)[0]);
}

void Program::setUniform(std::string name, glm::mat4 *m) {
	GLuint id = glGetUniformLocation(programID, name.c_str());
	if (id == -1)return;
	glUniformMatrix4fv(id, 1, GL_FALSE, &(*m)[0][0]);
}

void Program::setUniform(std::string name, float f) {
	GLuint id = glGetUniformLocation(programID, name.c_str());
	if (id == -1)return;
	glUniform1f(id, f);
}

void Program::setUniform(std::string name, int i) {
	GLuint id = glGetUniformLocation(programID, name.c_str());
	if (id == -1)return;
	glUniform1i(id, i);
}
void Program::setUniform(std::string name,unsigned int i) {
	GLuint id = glGetUniformLocation(programID, name.c_str());
	if (id == -1)return;
	glUniform1ui(id, i);
}


void Program::bind() {
	glUseProgram(programID);
}

GLuint Program::getProgramID() {
	return programID;
}

Shader Program::GetShader(GLuint type) {
	switch (type) {
	case GL_VERTEX_SHADER:
		return vertex;

	case GL_FRAGMENT_SHADER:
		return fragment;

	case GL_GEOMETRY_SHADER:
		return geometry;
	}
	return Shader();
}
