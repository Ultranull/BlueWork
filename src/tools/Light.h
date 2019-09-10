#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Buffer.h"

namespace lights {

#define MAXLIGHTS 10

	struct Light {
		struct attunation {
			float quadratic = 0;
			float linear = 0;
			float constant = 0;
			float padd = 0;
		};
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
	};

	struct Point : public Light {
		glm::vec4 position;

	};
	struct Directional : public Light {
		glm::vec4 direction;
	};
	struct Spot : public Light {
		glm::vec4 position;
		glm::vec4 direction;
		float cutOff;
	};

	class LightManager {
		UniformBuffer pointLightBuffer;
		std::vector<Point> pointLights;

		UniformBuffer directionalLightBuffer;
		std::vector<Directional> directionalLights;

		UniformBuffer spotLightBuffer;
		std::vector<Spot> spotLights;

	public:

		LightManager() {
			pointLights.reserve(MAXLIGHTS);
			directionalLights.reserve(MAXLIGHTS);
			spotLights.reserve(MAXLIGHTS);


		}

		void add(Point p);
		void add(Directional d);
		void add(Spot s);

	};
}