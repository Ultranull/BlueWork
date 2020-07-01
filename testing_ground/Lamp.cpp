#include "Lamp.h"




void Lamp::BuildSubTree(Geometry* geometry, Material mat, Light::PointData lightData) {
	LightSource = new PointLight(lightData);
	Mesh = new Entity(geometry, mat);

	LightSource->setName(name + "_PointLight");
	Mesh->setName(name + "_Entity");
	Mesh->transform.scale(glm::vec3(.5));

	add(LightSource);
	add(Mesh);
}

void Lamp::RegisterSerializer() {
	Serializer& S = Serializer::getInstance();
	Resource& R = Resource::getInstance();
	ComposeFunction entityCompose = [&](Node* node) {
		Lamp* lamp = (Lamp*)node;
		nlohmann::json json;

		nlohmann::json lightJson = S.Compose("PointLight", lamp->findByName<PointLight>("_PointLight"));



		json.merge_patch(S.Compose("Node", node));

		return json;
	};
	ParseFunction entityParse = [&](nlohmann::json data, Node* node, Node* parent) -> Node* {
		Lamp* lamp = (Lamp*)node;
		if (lamp == nullptr) {
			lamp = new Lamp();
		}

		std::string geometryName = data["Geometry"].get<std::string>();
		Material material = S.GeneralParse<Material>(data["Material"]);
		Light::PointData lightData;

		lightData.ambient = S.GeneralParse<glm::vec4>(data["Ambient"]);
		lightData.color = S.GeneralParse<glm::vec4>(data["Color"]);
		lightData.specular = S.GeneralParse<glm::vec4>(data["Specular"]);
		lightData.att = S.GeneralParse<Light::attunation>(data["Att"]);

		S.Parse("Node", data, lamp, parent);

		lamp->BuildSubTree(R.getGeometry(geometryName), material, lightData);

		return lamp;
	};
	S.RegisterParser("Lamp", entityCompose, entityParse);
}