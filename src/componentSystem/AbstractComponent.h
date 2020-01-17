#pragma once

/*
    can be independant or dependant?
*/
class AbstractComponent {
public:

    virtual void init() = 0;
	virtual void CleanUp() = 0;
};