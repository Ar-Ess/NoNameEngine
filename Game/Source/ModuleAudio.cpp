#include "ModuleAudio.h"
#pragma comment (lib, "Source/External/OpenAL/libs/debug/OpenAL/OpenAL32.lib")

#define OpenAL_ErrorCheck(message)\
{\
	ALenum error = alGetError();\
	if( error != AL_NO_ERROR)\
	{\
		std::cerr << "OpenAL Error: " << error << " with call for " << #message << std::endl;\
	}\
}

#define alec(FUNCTION_CALL)\
FUNCTION_CALL;\
OpenAL_ErrorCheck(FUNCTION_CALL)

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

void ModuleAudio::CreateListener()
{
	alec(alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f));
	alec(alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f));
}

