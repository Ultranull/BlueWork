#pragma once

#include <map>
#include <string>
#include <memory>

#include "AbstractSystem.h"

/*
    singleton managing systems
    components register to systems
*/

class SystemManager {

    std::map<std::string, std::unique_ptr<SystemInterface>> systems;

    SystemManager() {}

public:
    static SystemManager& getInstance() {
        static SystemManager instance;
        return instance;
    }

    SystemManager(SystemManager const&) = delete;
    void operator=(SystemManager const&) = delete;

    void RegisterSystem(std::string name, SystemInterface* system);

    template<typename ComponentType>
    ComponentType* CreateComponent();

    void start();
    void update();

	void CleanUp();

};