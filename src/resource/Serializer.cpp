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

nlohmann::json Serializer::Compose(std::string name, Node* node) {
	return ParseMap[name].Compose(node);
}

void Serializer::LoadFile(std::string fileName, SceneManager* manager) {
	std::string contents = Utilities::readFile(fileName.c_str());


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
