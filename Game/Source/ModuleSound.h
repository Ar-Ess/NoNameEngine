#pragma once
#include "Globals.h"
#include "Module.h"
#include "AudioSystem.h"


class AudioSystem;

class ModuleSound : public Module
{
public:

	ModuleSound(Application* app, bool start_enabled = true);
	~ModuleSound();

public:

	bool Init();
	bool Start();
	update_status Update();
	bool CleanUp();

	ALuint CreateBuffers();
	ALuint LoadStereo();
	ALuint CreateMonoSource(ALuint monoBuffer);
	ALuint CreateStereoSource(ALuint stereoBuffer);
	void PlayMonoSound(ALuint monoSource);
	void PlayStereoSound(ALuint stereoSource);

private:

	void InitAudio();
	static void ListAudioDevices(const ALCchar* devices);
	void CreateListener();
	void CleanUp(ALuint monoBuffer, ALuint stereoBuffer, ALuint monoSource, ALuint stereoSource);
	void CleanUpSource(ALuint source);
	void CleanUpBuffer(ALuint buffer);

public:

	AudioSystem* audio; // pointer to our audio system
	ALuint monoBuffer;
	ALuint stereoBuffer;
	ALuint monoSource;
	ALuint stereoSource;
	ReadWav monoData;
	ALCdevice* device;
	ALCcontext* context;
};