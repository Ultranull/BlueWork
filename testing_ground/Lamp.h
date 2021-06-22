#pragma once

#include "scene/Node.h"
#include "scene/Light.h"
#include "scene/Entity.h"

#include "resource/Serializer.h"
#include "resource/Resource.h"

class Lamp: public Node{

	PointLight* LightSource;
	Entity* Mesh;
	
public:
	static const std::string LightName;
	static const std::string MeshName;

	Lamp() :
		Node("Lamp",NodeType::Node) {

		//DebugGui::PushDraw(&Lamp::guidraw, this);
	}

	void BuildSubTree(std::shared_ptr<Geometry> geometry, Material mat, Light::PointData lightData);

	void guidraw();

	PointLight* GetLightSource() const;

	Entity* GetMesh() const;

	static void RegisterSerializer();
};