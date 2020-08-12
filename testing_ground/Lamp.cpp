#include "Lamp.h"

const std::string Lamp::LightName = "_PointLight";
const std::string Lamp::MeshName = "_Mesh";

void Lamp::BuildSubTree(Geometry* geometry, Material mat, Light::PointData lightData) {
	LightSource = new PointLight(lightData);
	Mesh = new Entity(geometry, mat);

	LightSource->setName(name + Lamp::LightName);
	Mesh->setName(name + Lamp::MeshName);
	Mesh->transform.scale(glm::vec3(.5));
	LightSource->SetDependancy(true);
	Mesh->SetDependancy(true);

	add(LightSource);
	add(Mesh);
}

void Lamp::guidraw() {
	ImGui::Begin("window"); {
		if (parent != nullptr) {
			if (ImGui::TreeNode((name + std::string(":") + std::to_string(Id) + std::string(":") + TypeName).c_str())) {
				ImGui::Indent();
				ImGui::DragFloat3(
					(std::string("position") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&transform.Position()));

				ImGui::ColorEdit3(
					(std::string("specular") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&LightSource->data.specular));

				if (ImGui::ColorEdit3(
					(std::string("color") + std::string("##") + std::to_string(Id)).c_str(),
					reinterpret_cast<float*>(&LightSource->data.color))) {
					Mesh->material.color = LightSource->data.color;
				}
				ImGui::Unindent();
				ImGui::TreePop();
			}
		}
	}ImGui::End();
}

inline PointLight* Lamp::GetLightSource() const {
	return LightSource;
}

inline Entity* Lamp::GetMesh() const {
	return Mesh;
}

void Lamp::RegisterSerializer() {
	Serializer& S = Serializer::getInstance();
	Resource& R = Resource::getInstance();
	ComposeFunction entityCompose = [&](Node* node) {
		Lamp* lamp = (Lamp*)node;
		nlohmann::json json;

		PointLight* light = lamp->GetLightSource();
		Entity* mesh = lamp->GetMesh();

		json.merge_patch(S.Compose(light));
		json.merge_patch(S.Compose(mesh));

		json.merge_patch(S.Compose("Node", node));

		return json;
	};
	ParseFunction entityParse = [&](nlohmann::json data, Node* node, Node* parent) -> Node* {
		Lamp* lamp = (Lamp*)node;
		if (lamp == nullptr) {
			lamp = new Lamp();
		}

		S.Parse("Node", data, lamp, parent);

		std::string geometryName = data.value("Geometry", "");
		Material material = S.GeneralParse<Material>(data["Material"]);
		Light::PointData lightData;

		lightData.ambient = S.GeneralParse<glm::vec4>(data["Ambient"]);
		lightData.color = S.GeneralParse<glm::vec4>(data["Color"]);
		lightData.specular = S.GeneralParse<glm::vec4>(data["Specular"]);
		lightData.att = S.GeneralParse<Light::attunation>(data["Att"]);
		lightData.shadowId = -1;
		lightData.FarPlane = 100;


		if (lamp->getNumberOfChildren() == 0) {
			lamp->BuildSubTree(R.getGeometry(geometryName), material, lightData);
		}
		else {
			PointLight* light = lamp->GetLightSource();
			light->data = lightData;
			light->CreatesShadow = data.value("Shadow", false);
			//S.Parse(light->GetTypeName(), data, light, nullptr); // nice but doesnt work right now. it parses the transform

			Entity* mesh = lamp->GetMesh();
			mesh->material = material;
			mesh->geometry = R.getGeometry(geometryName);
		}


		return lamp;
	};
	S.RegisterParser("Lamp", entityCompose, entityParse);
}