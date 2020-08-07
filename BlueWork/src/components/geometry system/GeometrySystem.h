#pragma once

#include "componentSystem/AbstractSystem.h"
#include "componentSystem/SystemManager.h"

#include "GeometryComponent.h"

class GeometrySystem: public AbstractSystem<GeometryComponent>{

public:
	inline static const std::string Name = "GeometrySystem";

	GeometrySystem(){}

	void OnStart() override;

	void OnUpdate() override;

	GeometryComponent* Register() override;

	// how does the renderer get a geometry component?
};

