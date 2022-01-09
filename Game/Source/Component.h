#ifndef __COMPONENT_H__
#define __COMPONENT_H__

#include <string>

enum ComponentID
{
	AUDIO_SOURCE_COMPONENT,
	NO_COMPONENT
};

class Component
{
public:
	~Component() {}

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
};

#endif // !__COMPONENT_H__

