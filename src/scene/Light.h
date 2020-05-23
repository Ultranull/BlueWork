#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Node.h"


enum class LightType {
	Point,
	Directional,
	Spot
};

struct Light : public Node {
	struct attunation {
		float quadratic = 0;
		float linear = 0;
		float constant = 0;
	};
	struct PointData {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		float padd = 0;
		glm::vec4 position;
	};
	struct DirectionalData {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		float padd = 0;
		glm::vec4 direction;
	};
	struct SpotData {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		float cutOff;
		glm::vec4 position;
		glm::vec4 direction;
	};

	LightType lightType;

	Light(std::string typeName):
		Node(typeName, NodeType::Light){}

	void cleanup() {
		Node::cleanup();
	}

	//virtual uint8_t* pack();

};

struct PointLight : public Light {
	PointData data;

	PointLight() :
		PointLight(PointData()) {
	}

	PointLight(PointData data):
		Light("PointLight"), data(data){
		lightType = LightType::Point;
		transform.Position() = glm::vec3(data.position);
	}

	PointData pack() {
		data.position = transform.FinalTransform(false) * glm::vec4(0, 0, 0, 1.);
		return data;
	}

};


