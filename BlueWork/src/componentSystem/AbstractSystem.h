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
		//std::vector<std::unique_ptr<ComponentType>>::iterator iter;
		for (auto iter = components.begin(); iter != components.end(); iter++) {
			(*iter)->CleanUp();
		}
		components.clear();
		components.shrink_to_fit();
	}

	void Remove(ComponentType* ptr) {
		//std::vector<std::unique_ptr<ComponentType>>::iterator iter;
		for (auto iter = components.begin(); iter != components.end(); ) {
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


	int NumberOfComponents() {
		return components.size();
	}

	ComponentType* Get(int i) {
		return components[i].get();
	}

	virtual ComponentType* Register() = 0;
};
