#pragma once

#include <map>
#include <string>
#include <memory>

#include <loguru.hpp>

#include "AbstractSystem.h"

#include "Utilities/Utilities.h"

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

    static SystemInterface* GetSystem(std::string name) {
        SystemManager& manager = getInstance();
        if (MapContains(manager.systems, name))
            return manager.systems[name].get();

        LOG_F(ERROR, "System \"%s\" does not exist", name);
        return nullptr;
    }

    template<typename ComponentType>
    ComponentType* CreateComponent();

    void start();
    void update();

	void CleanUp();

};