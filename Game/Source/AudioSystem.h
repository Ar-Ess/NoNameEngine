#pragma once

#include <iostream>
#include "External/OpenAL/include/AL/al.h"
#include "External/OpenAL/include/AL/alc.h"
#include "External/OpenAL/include/AL/alext.h"
#include <dr_wav.h>
#include <vector>

struct ReadWav
{
	unsigned int channels = 0;
	unsigned int sampleRate = 0;
	drwav_uint64 totalPCMFrameCount = 0;
	std::vector<uint16_t> pcmData;
	drwav_uint64 getTotalSamples() { return totalPCMFrameCount * channels; }
};

class AudioSystem
{
public:

	AudioSystem();
	~AudioSystem();

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

	ReadWav monoData;
	ALCdevice* device;
	ALCcontext* context;
};