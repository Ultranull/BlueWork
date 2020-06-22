#include "Entity.h"

#include "Utilities/DebugGui.h"

Entity::Entity(std::string typeName, Geometry* geom, Material mat) :
	geometry(geom), material(mat), flags(0), Node(typeName, NodeType::Entity) {

	DebugGui::PushDraw(&Entity::guidraw, this);
}

void Entity::cleanup() {
	Node::cleanup();
	geometry->cleanup();
	material.cleanup();
}

void Entity::guidraw() {
	ImGui::Begin("window"); {
		if (ImGui::TreeNode((name + std::string(":") + std::to_string(Id) + std::string(":") + TypeName).c_str())) {
			ImGui::Indent();
			ImGui::DragFloat3(
				(std::string("position") + std::string("##") + std::to_string(Id)).c_str(),
				reinterpret_cast<float*>(&transform.Position()));
			ImGui::DragFloat3(
				(std::string("scale") + std::string("##") + std::to_string(Id)).c_str(),
				reinterpret_cast<float*>(&transform.Scale()));
			ImGui::DragFloat(
				(std::string("shine") + std::string("##") + std::to_string(Id)).c_str(),
				&material.shininess, 1,0);

			ImGui::ColorEdit3(
				(std::string("color") + std::string("##") + std::to_string(Id)).c_str(),
				reinterpret_cast<float*>(&material.color));
			ImGui::Unindent();
			ImGui::TreePop();
		}
	}ImGui::End();
}
