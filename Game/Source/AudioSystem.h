#pragma once

#include <iostream>
#include "External/OpenAL/include/AL/al.h"
#include "External/OpenAL/include/AL/alc.h"
#include "External/OpenAL/include/AL/alext.h"
#include <dr_wav.h>
#include <dr_mp3.h>
#include <dr_flac.h>
#include <vector>
#include <string>
#include "External/OpenAL/include/AL/efx.h"
#include "External/OpenAL/include/AL/efx-creative.h"
#include "External/OpenAL/include/AL/efx-presets.h"

enum AudioFormat
{
	MP3,
	WAV,
	FLAC
};

class Track
{
public: // Methods
	uint64_t getTotalSamples()
	{
		uint64_t ret = 0;

		switch (format)
		{
		case WAV: ret = totalPCMFrameCountWav * channels; break;
		case MP3: ret = totalPCMFrameCountMp3 * channels; break;
		case FLAC: ret = totalPCMFrameCountFlac * channels; break;
		}

		return ret;
	}

	void SetPCMFrameCount(drwav_uint64 value)
	{
		switch (format)
		{
		case WAV: totalPCMFrameCountWav = value; break;
		case MP3: totalPCMFrameCountMp3 = (drmp3_uint64)value; break;
		case FLAC: totalPCMFrameCountFlac = (drflac_uint64)value; break;
		}
	}

public: // Variables
	unsigned int channels = 0;
	unsigned int sampleRate = 44100;
	unsigned int bits = 16;
	float duration;
	AudioFormat format;
	ALuint buffer;
	ALuint source;
	std::vector<uint16_t> pcmData;
	std::string path;
	std::string name;

private: // Variables

	drwav_uint64 totalPCMFrameCountWav = 0;
	drmp3_uint64 totalPCMFrameCountMp3 = 0;
	drflac_uint64 totalPCMFrameCountFlac = 0;
};

class AudioSystem
{
public:

	AudioSystem();
	~AudioSystem();

	void InitAudio();
	void CreateListener();

	Track LoadAudio(const char* path);
	ALuint CreateAudioSource(ALuint audioBuffer, bool spacial);
	void PlayAudio(ALuint audioSource, float time = 0.0f);
	void StopAudio(ALuint audioSource);
	void PauseAudio(ALuint audioSource);

	void CleanUp(ALuint buffer, ALuint source);

	bool SameString(std::string a, std::string b)
	{
		bool ret = true;

		if (a.size() != b.size()) return false;

		for (unsigned int i = 0; i < a.size(); i++)
		{
			ret = (a[i] == b[i]);
			if (!ret) return false;
		}

		return true;
	}

private: // Methods

	void ListAudioDevices(const ALCchar* devices);

	Track LoadMP3(const char* path);
	Track LoadWav(const char* path);
	Track LoadFlac(const char* path);

	void CleanUpSource(ALuint source);
	void CleanUpBuffer(ALuint buffer);

private:

	ALCdevice* device;
	ALCcontext* context;
};