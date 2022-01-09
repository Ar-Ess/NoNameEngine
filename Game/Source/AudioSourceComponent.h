#ifndef __AUDIO_SOURCE_COMPONENT_H__
#define __AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent() : Component("Audio Source", ComponentID::AUDIO_SOURCE_COMPONENT)
	{}
	~AudioSourceComponent() {}

private:

};

#endif // !__AUDIO_SOURCE_COMPONENT_H__
