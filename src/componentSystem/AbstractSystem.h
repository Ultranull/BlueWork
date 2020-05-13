#pragma once
#include <vector>

class SystemInterface {

public:
    virtual void OnUpdate() {}
    virtual void OnStart() {}
    virtual void CleanUp() {}
};

template<typename ComponentType>
class AbstractSystem : public SystemInterface {
protected:
    std::vector<ComponentType> components;

public:

    virtual void OnUpdate() {}
    virtual void OnStart() {}

	virtual void CleanUp() {
		std::vector<ComponentType>::iterator iter;
		for (iter = components.begin(); iter != components.end(); iter++) {
			iter->CleanUp();
		}
	}

	void Remove(ComponentType* ptr) {
		std::vector<ComponentType>::iterator iter;
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
