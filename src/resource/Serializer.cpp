#include "Serializer.h"

#include <loguru.hpp>

#include "scene/SceneManager.h"
#include "resource/Resource.h"
#include "Utilities.h"

#include "resource/CommonSerializers.h"

std::string Serializer::AsciiExtention = ".scene";

Serializer::Serializer() {
}

void Serializer::Initialize(){
	LOG_F(INFO, "Initializing Serializer");
	CommonSerializers::CommonParsers();
}

nlohmann::json Serializer::Compose(std::string type, Node* node) {
	if (ParseMap.find(type) != ParseMap.end()) {
		return ParseMap[type].Compose(node);
	}
	LOG_F(ERROR, "Could not compose type: %s", type.c_str());
	return nlohmann::json();
}

nlohmann::json Serializer::Compose(Node* node) {
	return Compose(node->GetTypeName(), node);
}

Node* Serializer::Parse(std::string type, nlohmann::json json, Node* node, Node* parent) {
	if (ParseMap.find(type) != ParseMap.end()) {
		return ParseMap[type].Parse(json, node, parent);
	}
	LOG_F(ERROR, "Could not parse type: %s with %s", type.c_str(), json.dump().c_str());
	return node;
}

Node* Serializer::Parse(std::string type, nlohmann::json json){
	return Parse(type, json, nullptr, nullptr);
}

void Serializer::LoadFile(std::string fileName, SceneManager* manager) {
	Resource& R = Resource::getInstance();
	std::string contents = Utilities::readFile(fileName.c_str());
	nlohmann::json json = nlohmann::json::parse(contents);

	R.batchLoad(json["Manifest"].get<std::string>());
	Node* root = manager->GetRoot();
	Parse(root->GetTypeName(), json["Scene"], root, nullptr);

}

void Serializer::SaveFile(std::string fileName, SceneManager* manager) {
	Resource& R = Resource::getInstance();
	std::string manifest = R.GetManifest();
	
	nlohmann::json saveContent = GeneralCompose(manager);
	saveContent["Manifest"] = manifest;

	if(!fileName.ends_with(AsciiExtention.c_str())){
		fileName += AsciiExtention;
	}

	LOG_F(INFO, "Saving %s", fileName.c_str());
	LOG_F(INFO + 1, "With Manifest: %s", manifest.c_str());
	Utilities::SaveFile(fileName.c_str(), saveContent.dump().c_str());
}

void Serializer::RegisterParser(std::string name, ComposeFunction compose, ParseFunction parse) {
	Serializer& instance = getInstance();
	LOG_F(INFO + 1, "registering %s with serializer", name.c_str());
	instance.ParseMap.insert(std::make_pair(name, ParseIdentity{ compose, parse }));
}
