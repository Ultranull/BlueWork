#include "SystemManager.h"

#include <loguru.hpp>

void SystemManager::RegisterSystem(std::string name, SystemInterface* system) {
    LOG_F(INFO, "Registering system %s", name.c_str());
    systems.insert(std::make_pair(name, std::unique_ptr<SystemInterface>(system)));
}

void SystemManager::start() {
    std::map<std::string, std::unique_ptr<SystemInterface>>::iterator it;
    for (it = systems.begin(); it != systems.end(); it++) {
        it->second->OnStart();
    }
}

void SystemManager::update() {
    std::map<std::string, std::unique_ptr<SystemInterface>>::iterator it;
    for (it = systems.begin(); it != systems.end(); it++) {
        it->second->OnUpdate();
    }
}

void SystemManager::CleanUp() {
    LOG_F(INFO+1, "Cleaning system manager");
	std::map<std::string, std::unique_ptr<SystemInterface>>::iterator it;
	for (it = systems.begin(); it != systems.end(); it++) {
		it->second->CleanUp();
	}
    systems.clear();
}