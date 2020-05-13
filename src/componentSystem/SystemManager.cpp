#include "SystemManager.h"

#include <loguru.hpp>

void SystemManager::RegisterSystem(std::string name, SystemInterface* system) {
    LOG_F(INFO, "Registering system %s", name);
    systems.insert(std::make_pair(name, system));
}

void SystemManager::start() {
    std::map<std::string, SystemInterface*>::iterator it;
    for (it = systems.begin(); it != systems.end(); it++) {
        it->second->OnStart();
    }
}

void SystemManager::update() {
    std::map<std::string, SystemInterface*>::iterator it;
    for (it = systems.begin(); it != systems.end(); it++) {
        it->second->OnUpdate();
    }
}

void SystemManager::CleanUp() {
    LOG_F(INFO+1, "Cleaning system manager");
	std::map<std::string, SystemInterface*>::iterator it;
	for (it = systems.begin(); it != systems.end(); it++) {
		it->second->CleanUp();
	}
}