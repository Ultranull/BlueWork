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

	Lamp::Lamp() :
		Node("Lamp",NodeType::Node) {

		//DebugGui::PushDraw(&Lamp::guidraw, this);
	}

	void BuildSubTree(Geometry* geometry, Material mat, Light::PointData lightData);

	void guidraw();

	static void RegisterSerializer();
};