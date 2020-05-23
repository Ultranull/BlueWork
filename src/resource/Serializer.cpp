#include "Serializer.h"

#include "scene/SceneManager.h"
#include "resource/Resource.h"
#include "Utilities.h"

//todo where do i run this for any given node?

Serializer::Serializer() {
}

nlohmann::json Serializer::Compose(std::string name, Node* node) {
	return ParseMap[name].Compose(node);
}

void Serializer::LoadFile(std::string fileName, SceneManager& manager) {
	std::string contents = Utilities::readFile(fileName.c_str());


}

void Serializer::RegisterParser(std::string name, ComposeFunction compose, ParseFunction parse) {
	Serializer& instance = getInstance();
	instance.ParseMap.insert(std::make_pair(name, ParseIdentity{ compose, parse }));
}

template<>
nlohmann::json Serializer::GeneralCompose(glm::vec3 object) {
	nlohmann::json json = {
		{"x",object.x},
		{"y",object.y},
		{"z",object.z} };
	return json;
}

template<>
nlohmann::json Serializer::GeneralCompose(glm::vec4 object) {
	nlohmann::json json = {
		{"x",object.x},
		{"y",object.y},
		{"z",object.z},
		{"w",object.w} };
	return json;
}

template<>
nlohmann::json Serializer::GeneralCompose(glm::quat object) {
	nlohmann::json json = {
		{"x",object.x},
		{"y",object.y},
		{"z",object.z},
		{"w",object.w} };
	return json;
}

void Serializer::CommonParsers() {
	Resource& R = Resource::getInstance();
	ComposeFunction nodeCompose =
		[&](Node* node) {
		nlohmann::json json;

		json["Id"] = node->GetId();
		json["Transform"] = GeneralCompose<Transform>(node->transform);
		json["name"] = node->getName();
		json["parent"] = node->GetParentId();
		nlohmann::json childrenArray = nlohmann::json::array();
		for (int i = 0; i < node->getNumberOfChildren(); i++) {
			Node* child = node->child(i);
			childrenArray.push_back(child->GetId());
		}
		json["children"] = childrenArray;
		return json;
	};
	ParseFunction nodeParse =
		[&](nlohmann::json data) -> Node* {
		return nullptr;
	};
	RegisterParser("Node", nodeCompose, nodeParse);

	ComposeFunction entityCompose =
		[&](Node* node) {
		Entity* entity = (Entity*)node;
		nlohmann::json json;

		json["flags"] = entity->flags;
		json["Geometry"] = R.GetGeometryName(entity->geometry);
		json["Material"] = GeneralCompose<Material>(entity->material);

		json.merge_patch(Compose("Node", node));

		return json;
	};
	ParseFunction entityParse =
		[&](nlohmann::json data) -> Node* {
		return nullptr;
	};
	RegisterParser("Entity", entityCompose, entityParse);

	ComposeFunction cameraCompose =
		[&](Node* node) {
		Camera* camera = (Camera*)node;
		nlohmann::json json;

		json["Settings"] = GeneralCompose(camera->GetSettings());

		json.merge_patch(Compose("Node", node));

		return json;
	};
	ParseFunction cameraParse =
		[&](nlohmann::json data) -> Node* {
		return nullptr;
	};
	RegisterParser("Camera", cameraCompose, cameraParse);
}