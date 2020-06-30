#pragma once


#include <glad/glad.h>
#include <glm/glm.hpp>


struct Engine {
	struct Vertex {
		glm::vec3 Position = glm::vec3(0);
		glm::vec2 TextureCoord = glm::vec2(0);
		glm::vec3 Normal = glm::vec3(0);
		glm::vec3 Tangent = glm::vec3(0);

		Vertex(glm::vec3 position, glm::vec2 textureCoord, glm::vec3 normal) :
			Position(position), TextureCoord(textureCoord), Normal(normal) {}
	};

	enum VertexDataPosition :unsigned short {
		POSITION = 0,
		TEXTURECOORD,
		NORMAL,
		TANGENT,
		OFFSET,
		BONEID,
		BONDEWEIGHT,

		OPEN
	};

	enum TextureMapLocation : unsigned int {
		NORMALMAP = GL_TEXTURE0,
		SPECULAR,
		BUMP,
		PASSMAP,
		DIFFUSE,

		OPENTEXTURE
	};


};