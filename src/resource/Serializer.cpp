#include "Serializer.h"

#include "Utilities.h"

//todo where do i run this for any given node?
//todo how to handle inhertance

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

void Serializer::CommonParsers() {
	ComposeFunction nodeCompose =
		[&](Node* node) {
		nlohmann::json json;

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
		json["Geometry"] = "none for now";
		json["Material"] = GeneralCompose<Material>(entity->material);

		json.merge_patch(Compose("Node", node));

		return json;
	};
	ParseFunction entityParse =
		[&](nlohmann::json data) -> Node* {
		return nullptr;
	};
	RegisterParser("Entity", entityCompose, entityParse);
}