#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>


struct Engine {
	struct Vertex {
		glm::vec3 pos = glm::vec3(0);
		glm::vec3 color = glm::vec3(0);
		glm::vec2 UV = glm::vec2(0);
		glm::vec3 normal = glm::vec3(0);
	};

	typedef enum VertexDataPosition :unsigned short {
		POSITION = 0,
		COLOR,
		TEXTURECOORD,
		NORMAL,
		OFFSET,
		BONEID,
		BONDEWEIGHT,

		OPEN
	};

	enum TextureMapLocation : unsigned int {
		DIFFUSE = GL_TEXTURE0,
		NORMALMAP,
		SPECULAR,
		BUMP,
		PASSMAP
	};


};