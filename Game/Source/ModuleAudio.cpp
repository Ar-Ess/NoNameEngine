#include "ModuleAudio.h"
#pragma comment (lib, "Source/External/OpenAL/libs/debug/OpenAL/OpenAL32.lib")

ModuleAudio::ModuleAudio()
{
}

ModuleAudio::~ModuleAudio()
{
}

void ModuleAudio::InitAudio()
{
	const ALCchar* defaultDeviceStr = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	ALCdevice* device = alcOpenDevice(defaultDeviceStr);
	if (!device)
	{
		LOG("Failed to get the default device for OpenAL");

	}
	LOG("OpenAL Device", alcGetString(device, ALC_DEVICE_SPECIFIER));

	ALCcontext* context = alcCreateContext(device, nullptr);

	if (!alcMakeContextCurrent(context))
	{
		LOG("Failed to make the OpenAL context the current context");
	}
}

