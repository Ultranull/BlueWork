#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Node.h"

namespace lights {

#define MAXLIGHTS 10

	struct attunation {
		float quadratic = 0;
		float linear = 0;
		float constant = 0;
		float padd = 0;
	};
	struct Point {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		glm::vec4 position;
	};
	struct Directional {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		glm::vec4 direction;
	};
	struct Spot {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		glm::vec4 position;
		glm::vec4 direction;
		float cutOff;
	};

	struct Light : public Node {
		union {
			Spot spotLight;
			Point pointLight;
			Directional dirLight;
		};
		Light() {}
		Light(Point p) :
			Node(NodeType::POINTLIGHT), pointLight(p) {}
		void cleanup() {
			Node::cleanup();
		}

		void SetPosition(glm::vec3 pos);
	};



}