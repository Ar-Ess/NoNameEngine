#include "AudioSystem.h"
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

AudioSystem::AudioSystem()
{
	context = 0;

}

AudioSystem::~AudioSystem()
{

}

void AudioSystem::InitAudio()
{
	device = alcOpenDevice(NULL);
	if (!device)
	{
		//LOG("Failed to get the default device for OpenAL");
	}
	//LOG("OpenAL Device", alcGetString(device, ALC_DEVICE_SPECIFIER));

	ListAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	ALboolean enumeration;
	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT");
	if (enumeration == AL_FALSE)
	{
		//LOG("Enumeration not supported");
	}
	context = alcCreateContext(device, nullptr);

	ALboolean bEAX = alIsExtensionPresent("EAX2.0");
	if (bEAX == AL_FALSE)
	{
		//LOG("EAX not supported");
	}

	if (!alcMakeContextCurrent(context))
	{
		//LOG("Failed to make the OpenAL context the current context");
	}
}

void AudioSystem::ListAudioDevices(const ALCchar* devices)
{
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	//LOG("Devices list: ");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		//LOG("%s", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}

Track AudioSystem::LoadAudio(const char* path)
{
	std::string audioPath = path;
	audioPath.erase(audioPath.begin(), audioPath.end() - 1);
	Track ret;

	if (SameString("3", audioPath.c_str()))
	{
		LoadMP3(path);
	}
	else if (SameString("v", audioPath.c_str()))
	{
		ret = LoadWav(path);
	}
	else if (SameString("c", audioPath.c_str()))
	{

	}

	return ret;
}

ALuint AudioSystem::CreateAudioSource(ALuint audioBuffer, bool mono)
{
	ALuint source;
	alec(alGenSources(1, &source));
	if (mono)
	{
		alec(alSource3f(source, AL_POSITION, 1.0f, 0.0f, 0.0f));
		alec(alSource3f(source, AL_VELOCITY, 0.f, 0.f, 0.f));
	}
	alec(alSourcef(source, AL_PITCH, 1.f));
	alec(alSourcef(source, AL_GAIN, 1.f));
	alec(alSourcei(source, AL_LOOPING, AL_FALSE));
	alec(alSourcei(source, AL_BUFFER, audioBuffer));

	return source;
}

void AudioSystem::PlayAudio(ALuint audioSource)
{
	alec(alSourcePlay(audioSource));
	ALint sourceState;
	alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));
	while (sourceState == AL_PLAYING)
	{
		//basically loop until we're done playing the mono sound source
		alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));
	}
}

ALuint AudioSystem::LoadMP3(const char* path)
{
	//ReadTrack stereoData;
	//{
	//	drmp3_uint64 totalPCMFrameCountMp3;
	//	drwav_int16* pSampleData = drmp3_open_file_and_read_pcm_frames_s16(path, &stereoData.channels, &stereoData.sampleRate, &totalPCMFrameCountMp3, nullptr);
	//	if (pSampleData == NULL) {
	//		std::cerr << "failed to load audio file" << std::endl;
	//		return 0;
	//	}
	//	if (stereoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max))
	//	{
	//		std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
	//		return 0;
	//	}
	//	stereoData.pcmData.resize(size_t(stereoData.getTotalSamples()));
	//	std::memcpy(stereoData.pcmData.data(), pSampleData, stereoData.pcmData.size() * /*twobytes_in_s15*/2);
	//	drwav_free(pSampleData, nullptr);
	//}

	//ALuint stereoSoundBuffer;
	//alec(alGenBuffers(1, &stereoSoundBuffer));
	//alec(alBufferData(stereoSoundBuffer, stereoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, stereoData.pcmData.data(), stereoData.pcmData.size() * 2 /*two bytes per sample*/, stereoData.sampleRate));

	//return &stereoSoundBuffer;

	return 0;
}

Track AudioSystem::LoadWav(const char* path)
{
	Track track;
	drwav_uint64 totalPCMFrameCount = 0;

	track.format = WAV;
	track.path = path;
	drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(path, &track.channels, &track.sampleRate, &totalPCMFrameCount, nullptr);
	track.SetPCMFrameCount(totalPCMFrameCount);

	if (pSampleData == NULL)
	{
		std::cerr << "failed to load audio file" << std::endl;
		return Track();
	}

	if (track.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
	{
		std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
		return Track();
	}

	track.pcmData.resize(size_t(track.getTotalSamples()));
	std::memcpy(track.pcmData.data(), pSampleData, track.pcmData.size() * /*twobytes_in_s15*/2);
	drwav_free(pSampleData, nullptr);

	ALuint buffer;
	alec(alGenBuffers(1, &buffer));
	alec(alBufferData(buffer, track.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, track.pcmData.data(), track.pcmData.size() * 2 /*two bytes per sample*/, track.sampleRate));
	track.buffer = buffer;

	return track;
}

// ---------------------------

void AudioSystem::CreateListener()
{
	alec(alListener3f(AL_POSITION, 0.0f, 0.0f, 0.0f));
	alec(alListener3f(AL_VELOCITY, 0.0f, 0.0f, 0.0f));
	ALfloat forwardUpVec[] =
	{
		1.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};
	alec(alListenerfv(AL_ORIENTATION, forwardUpVec));
}

void AudioSystem::CleanUp(ALuint monoBuffer, ALuint stereoBuffer, ALuint monoSource, ALuint stereoSource)
{
	alec(alDeleteSources(1, &monoSource));
	alec(alDeleteSources(1, &stereoSource));
	alec(alDeleteBuffers(1, &monoBuffer));
	alec(alDeleteBuffers(1, &stereoBuffer));
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void AudioSystem::CleanUpSource(ALuint source)
{
	alec(alDeleteSources(1, &source));
}

void AudioSystem::CleanUpBuffer(ALuint buffer)
{
	alec(alDeleteSources(1, &buffer));
}

