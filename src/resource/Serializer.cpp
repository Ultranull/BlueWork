#include "Serializer.h"

#include <loguru.hpp>

#include "scene/SceneManager.h"
#include "resource/Resource.h"
#include "Utilities.h"

#include "resource/CommonSerializers.h"

Serializer::Serializer() {
}

void Serializer::Initialize(){
	LOG_F(INFO, "Initializing Serializer");
	CommonSerializers::CommonParsers();
}

nlohmann::json Serializer::Compose(std::string name, Node* node) {
	return ParseMap[name].Compose(node);
}

void Serializer::LoadFile(std::string fileName, SceneManager& manager) {
	std::string contents = Utilities::readFile(fileName.c_str());


}

void Serializer::RegisterParser(std::string name, ComposeFunction compose, ParseFunction parse) {
	Serializer& instance = getInstance();
	LOG_F(INFO + 1, "registering %s with serializer", name.c_str());
	instance.ParseMap.insert(std::make_pair(name, ParseIdentity{ compose, parse }));
}
