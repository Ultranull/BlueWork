#include "Node.h"

#include "SceneManager.h"
#include "Utilities/DebugGui.h"

#include <loguru.hpp>

unsigned int Node::NextId = 0;

Node::Node():
	Node("Node",NodeType::Node)
{}

Node::Node(std::string typeName, NodeType t) :
	Node(NextId++, typeName, t)
{}

Node::Node(unsigned int id, std::string typeName, NodeType t):
	children(), parent(nullptr), type(t), Id(id), TypeName(typeName),IsDependant(false)
{
	DebugGui::PushDraw(&Node::debuggui, this);
}

void JsonToDear(nlohmann::json& data, bool& updated) {
	int intkey = 0;
	bool isArray = false;
	for (auto& [key, val] : data.items()) {
		
		if (data.type() == nlohmann::json::value_t::array) {
			intkey = std::stoi(key); 
			isArray = true;
		}

		std::string oldData = (isArray ? data[intkey] : data[key]).dump();

		switch (val.type()) {
			case nlohmann::json::value_t::object:			
				if (ImGui::TreeNode(key.c_str())) {
					ImGui::Indent();
					JsonToDear((isArray ? data[intkey] : data[key]),updated);
					ImGui::Unindent();
					ImGui::TreePop();
				}
				break;
			case nlohmann::json::value_t::array:
				if (ImGui::TreeNode(key.c_str())) {
					ImGui::Indent();
					JsonToDear((isArray ? data[intkey] : data[key]), updated);
					ImGui::Unindent();
					ImGui::TreePop();
				}
				break;

			case nlohmann::json::value_t::string: {
				std::string value = val.get<std::string>();
				char buff[64] = "";
				std::memcpy(buff, value.c_str(), value.size());
				ImGui::InputText(key.c_str(), buff, 64);
				(isArray ? data[intkey] : data[key]) = buff;
			}break;

			case nlohmann::json::value_t::number_float: {
				float value = val.get<float>();
				ImGui::InputFloat(key.c_str(), &value, 0.01, 10, 2);
				(isArray ? data[intkey] : data[key]) = value;
			}break;

			case nlohmann::json::value_t::number_integer: {
				int value = val.get<int>();
				ImGui::InputInt(key.c_str(), &value);
				(isArray ? data[intkey] : data[key]) = value;
			}break;

			case nlohmann::json::value_t::boolean: {
				bool value = val.get<bool>();
				ImGui::Checkbox(key.c_str(), &value);
				(isArray ? data[intkey] : data[key]) = value;
			}break;

			default:
				ImGui::Text("%s: %s", key.c_str(), val.type_name());
				break;
			}

		std::string newData = (isArray ? data[intkey] : data[key]).dump();
		if (newData != oldData) {
			updated = true;
		}
	}
}

void Node::debuggui() {
	ImGui::Begin("window"); {

		if (ImGui::TreeNode((name + std::string(":") + std::to_string(Id) + std::string(":") + TypeName).c_str())) {
			ImGui::Indent();
			Serializer& S = Serializer::getInstance();
			nlohmann::json data = S.Compose(this);
			bool updated = false;
			JsonToDear(data, updated);

			if (updated) {
				S.Parse(TypeName, data, this, nullptr);
			}

			ImGui::TextWrapped("%s", data.dump().c_str());

			ImGui::Unindent();
			ImGui::TreePop();
		}
		
	}ImGui::End();
}

glm::mat4 Node::ResolveFinalTransform() {
	glm::mat4 parentTransform = glm::mat4(1);
	if (parent != nullptr) {
		if (!transform.IsValid()) {
			parentTransform = parent->ResolveFinalTransform();
		}
		else{
			parentTransform = parent->transform.FinalTransform(false);
		}
	}
	return transform.FinalTransform(true, parentTransform);
}

void Node::add(Node* child) {
	Node* childPtr = manager->PushNode(child);
	children.push_back(childPtr);
	childPtr->setParent(this);
	childPtr->setManager(manager);
}
void Node::setParent(Node* p) {
	parent = p;
}

int Node::getNumberOfChildren(){
	return children.size();
}

NodeType Node::GetType(){
	return type;
}

void Node::cleanup(){
	for (int i = 0; i < getNumberOfChildren(); i++)
		child(i)->cleanup();
}

unsigned int Node::GetParentId() {
	if (parent != nullptr) {
		return parent->GetId();
	}
	return -1;
}

unsigned int Node::GetId() {
	return Id;
}

Node* Node::child(GLuint index) {
	return children[index];
}

std::string Node::getName() {
	return name;
}

Node* Node::setName(std::string name) {
	this->name = name;
	return this;
}

std::string Node::GetTypeName() { return TypeName; }

void Node::SetDependancy(bool dependant) {
	IsDependant = dependant;
}

 bool Node::GetIsDependant() {
	return IsDependant;
}

void Node::setManager(SceneManager* sceneManager) {
	manager = sceneManager;
}

