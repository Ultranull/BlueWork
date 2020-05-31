#pragma once
#include <vector>
#include <memory>

class SystemInterface {

public:
    virtual void OnUpdate() {}
    virtual void OnStart() {}
    virtual void CleanUp() {}
};

template<typename ComponentType>
class AbstractSystem : public SystemInterface {
protected:
    std::vector<std::unique_ptr<ComponentType>> components;

public:

	AbstractSystem(): components() {}

    virtual void OnUpdate() {}
    virtual void OnStart() {}

	virtual void CleanUp() {
		std::vector<std::unique_ptr<ComponentType>>::iterator iter;
		for (iter = components.begin(); iter != components.end(); iter++) {
			(*iter)->CleanUp();
		}
		components.clear();
	}

	void Remove(ComponentType* ptr) {
		std::vector<std::unique_ptr<ComponentType>>::iterator iter;
		for (iter = components.begin(); iter != components.end(); ) {
			if (&(*iter) == ptr) {
				iter->CleanUp();
				ptr = nullptr;
				iter = components.erase(iter);
				break;
			}
			else {
				iter++;
			}
		}
	}

	virtual ComponentType* Register() = 0;
	//{
 //       components.push_back(ComponentType());
 //       return &components.back();
 //   }
};
