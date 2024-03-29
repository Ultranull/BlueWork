#pragma once

#include <map>
#include <vector>
#include <string>
#include<glad/glad.h>
#include "glm/glm.hpp"


struct Shader {
	GLuint id = -1;
	GLuint type = -1;
	std::string FileName;
	Shader() {}
	Shader(std::string file, GLuint t);
	inline bool operator==(const Shader& rhs) {
		return id == rhs.id;
	}
	void cleanup();
};
class Program {
	GLuint programID;
	Shader vertex, fragment, geometry;
public:
	Program(Shader vert, Shader frag);
	Program(Shader vert, Shader frag, Shader geom);
	Program():programID(0) {}
	~Program();

	void cleanup();

	void setUniform(std::string name, glm::vec2 *v);
	void setUniform(std::string name, glm::vec3 *v); 
	void setUniform(std::string name, glm::vec4 *v);
	void setUniform(std::string name, glm::mat4 *m);
	void setUniform(std::string name, float f);
	void setUniform(std::string name, int i);
	void setUniform(std::string name, unsigned int i);


	void bind();

	GLuint getProgramID();

	Shader GetShader(GLuint type);
};
