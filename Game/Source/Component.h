#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <string>
#include "Globals.h"
//#include "Shapes3D.h"
#include "Timer.h"

class Shape3D;

enum ComponentID
{
	AUDIO_SOURCE_COMPONENT,
	SPACIAL_AUDIO_SOURCE_COMPONENT,
	LINIAR_VELOCITY_COMPONENT,
	NO_COMPONENT
};

class Component
{
public:
	~Component() {}

	virtual void Start(Shape3D* afected) {}

	virtual void Update(Shape3D* afected) {}

	virtual void Draw(bool* onWindow = nullptr) {}

	virtual void End(Shape3D* afected) {}

	const char* GetTitle()
	{
		return title.c_str();
	}

protected:
	Component(const char* titleText, ComponentID componentId) 
	{
		title.clear();
		title += titleText;
		id = componentId;
	}

	std::string title;
	ComponentID id = NO_COMPONENT;
	bool open = false;
	Timer* gameTimer = nullptr;
};

#endif // !__COMPONENT_H__

