#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "Node.h"

#include "Utilities/DebugGui.h"


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
		float intensity = 0;
	};
	struct PointData {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		glm::vec4 position;
		float FarPlane;
		int shadowId;
		int shadow=false;
		float p=42.;

	};
	struct DirectionalData {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		glm::vec4 direction;
	};
	struct SpotData {
		glm::vec4 ambient;
		glm::vec4 color;
		glm::vec4 specular;
		attunation att;
		struct {
			float cutOff;
			float padding[3];
		};
		glm::vec4 position;
		glm::vec4 direction;
	};

	LightType lightType;

	bool CreatesShadow;

	Light(std::string typeName):
		CreatesShadow(false), Node(typeName, NodeType::Light){}

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
		//DebugGui::PushDraw(&PointLight::guidraw, this);
		data.shadow = false;
	}

	PointData pack() {
		data.position = transform.FinalTransform(false) * glm::vec4(0, 0, 0, 1.);
		data.shadow = CreatesShadow;
		return data;
	}
	void guidraw() {
		ImGui::Begin("window"); {
			if (ImGui::TreeNode((name + std::string(":") + std::to_string(Id) + std::string(":") + TypeName).c_str())) {
				ImGui::Indent();
				ImGui::DragFloat3(
					(std::string("position") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&transform.Position()));

				ImGui::ColorEdit3(
					(std::string("ambient") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&data.ambient));
				ImGui::ColorEdit3(
					(std::string("specular") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&data.specular));
				ImGui::DragFloat4(
					(std::string("att") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&data.att),.01);

				ImGui::ColorEdit3(
					(std::string("color") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&data.color));

				ImGui::Checkbox("shadow", &CreatesShadow);
				ImGui::Unindent();
				ImGui::TreePop();
			}
		}ImGui::End();
	}
};


