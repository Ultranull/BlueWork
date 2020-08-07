#include "GeometrySystem.h"


template<>
GeometryComponent* SystemManager::CreateComponent() {
	SystemInterface* si = systems[GeometrySystem::Name].get();
	GeometrySystem* cp = static_cast<GeometrySystem*>(si);

	return cp->Register();
}


GeometryComponent* GeometrySystem::Register() {
	components.push_back(std::make_unique<GeometryComponent>());
	return components.back().get();
}