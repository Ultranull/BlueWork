#include "SystemManager.h"


void SystemManager::RegisterSystem(std::string name, SystemInterface* system) {
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
	std::map<std::string, SystemInterface*>::iterator it;
	for (it = systems.begin(); it != systems.end(); it++) {
		it->second->CleanUp();
	}
}