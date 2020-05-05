#pragma once

/*
    can be independant or dependant?
*/
class AbstractComponent {
public:

    virtual void Initialize() = 0;
	virtual void CleanUp() = 0;
};