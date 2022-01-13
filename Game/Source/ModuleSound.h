#pragma once
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

private:

	void InitAudio();
	void CreateListener();
	ALuint* CreateBuffers();
	ALuint* LoadStereo();
	ALuint* CreateMonoSource(ALuint* monoBuffer);
	ALuint* CreateStereoSource(ALuint* stereoBuffer);
	void PlayMonoSound(ALuint* monoSource);
	void PlayStereoSound(ALuint* stereoSource);
	void CleanUp(ALuint monoBuffer, ALuint stereoBuffer, ALuint monoSource, ALuint stereoSource);

private:

	AudioSystem* audio; // pointer to our audio system
	ALuint* monoBuffer;
	ALuint* stereoBuffer;
	ALuint* monoSource;
	ALuint* stereoSource;

	ReadWav monoData;
	ALCdevice* device;
	ALCcontext* context;
};