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
		ret = LoadMP3(path);
	}
	else if (SameString("v", audioPath.c_str()))
	{
		ret = LoadWav(path);
	}
	else if (SameString("c", audioPath.c_str()))
	{
		ret = LoadFlac(path);
	}

	return ret;
}

ALuint AudioSystem::CreateAudioSource(ALuint audioBuffer, bool mono)
{
	ALuint source;
	alec(alGenSources(1, &source));
	alec(alSourcef(source, AL_PITCH, 1.f));
	alec(alSourcef(source, AL_GAIN, 1.f));
	alec(alSourcei(source, AL_LOOPING, AL_FALSE));
	alec(alSourcei(source, AL_BUFFER, audioBuffer));

	if (mono)
	{
		alec(alDistanceModel(AL_LINEAR_DISTANCE_CLAMPED));
		alSourcef(source, AL_ROLLOFF_FACTOR, 0.0f);
		alSourcei(source, AL_SOURCE_RELATIVE, AL_TRUE);
		alSource3f(source, AL_POSITION, 0, 0, 0);
	}

	return source;
}

void AudioSystem::PlayAudio(ALuint audioSource, float time)
{
	ALint sourceState;
	alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));

	if (sourceState == AL_STOPPED || sourceState == AL_INITIAL)
	{
		if (time != 0.0f) alSourcef(audioSource, AL_SEC_OFFSET, time);
		alec(alSourcePlay(audioSource));
	}
}

void AudioSystem::PauseAudio(ALuint audioSource)
{
	ALint sourceState;
	alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));

	if (sourceState == AL_PLAYING) alec(alSourcePause(audioSource));
}

void AudioSystem::StopAudio(ALuint audioSource)
{
	ALint sourceState;
	alec(alGetSourcei(audioSource, AL_SOURCE_STATE, &sourceState));

	if (sourceState == AL_PLAYING || sourceState == AL_PAUSED) alec(alSourceStop(audioSource));
}

Track AudioSystem::LoadMP3(const char* path)
{
	Track track;
	drmp3_uint64 totalPCMFrameCount = 0;
	drmp3_config mp3Config;

	std::string name = path;
	unsigned int offset = name.find_last_of('/') + 1;
	if (offset != 0) name.erase(name.begin(), name.begin() + offset);
	else
	{
		offset = name.find_last_of('\\') + 1;
		name.erase(name.begin(), name.begin() + offset);
	}

	track.format = MP3;
	track.path = path;
	track.name = name.c_str();
	drmp3_int16* pSampleData = drmp3_open_file_and_read_pcm_frames_s16(path, &mp3Config, &totalPCMFrameCount, nullptr);
	track.SetPCMFrameCount(totalPCMFrameCount);
	track.channels = mp3Config.channels;
	track.sampleRate = mp3Config.sampleRate;

	if (pSampleData == NULL)
	{
		std::cerr << "failed to load audio file" << std::endl;
		return Track();
	}

	track.bits = 16;

	if (track.getTotalSamples() > drmp3_uint64(std::numeric_limits<size_t>::max()))
	{
		std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
		return Track();
	}

	track.pcmData.resize(size_t(track.getTotalSamples()));
	std::memcpy(track.pcmData.data(), pSampleData, track.pcmData.size() * /*twobytes_in_s15*/2);
	drmp3_free(pSampleData, nullptr);

	ALuint buffer;
	alec(alGenBuffers(1, &buffer));
	alec(alBufferData(buffer, track.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, track.pcmData.data(), track.pcmData.size() * 2 /*two bytes per sample*/, track.sampleRate));
	
	ALint bufferSize;
	alec(alGetBufferi(buffer, AL_SIZE, &bufferSize));
	track.duration = bufferSize / (track.sampleRate * track.channels * 2); //(2 bytes == 16 Bits)
	
	track.buffer = buffer;

	return track;
}

Track AudioSystem::LoadWav(const char* path)
{
	Track track;
	drwav_uint64 totalPCMFrameCount = 0;

	std::string name = path;
	unsigned int offset = name.find_last_of('/') + 1;
	if (offset != 0) name.erase(name.begin(), name.begin() + offset);
	else
	{
		offset = name.find_last_of('\\') + 1;
		name.erase(name.begin(), name.begin() + offset);
	}

	track.format = WAV;
	track.path = path;
	track.name = name.c_str();
	drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16(path, &track.channels, &track.sampleRate, &totalPCMFrameCount, nullptr);
	track.SetPCMFrameCount(totalPCMFrameCount);

	if (pSampleData == NULL)
	{
		std::cerr << "failed to load audio file" << std::endl;
		return Track();
	}

	track.bits = 16;

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
	
	ALint bufferSize;
	alec(alGetBufferi(buffer, AL_SIZE, &bufferSize));
	track.duration = (float)bufferSize / (track.sampleRate * track.channels * 2); //(2 bytes == 16 Bits)

	track.buffer = buffer;

	return track;
}

Track AudioSystem::LoadFlac(const char* path)
{
	Track track;
	drflac_uint64 totalPCMFrameCount = 0;

	std::string name = path;
	unsigned int offset = name.find_last_of('/') + 1;
	if (offset != 0) name.erase(name.begin(), name.begin() + offset);
	else
	{
		offset = name.find_last_of('\\') + 1;
		name.erase(name.begin(), name.begin() + offset);
	}

	track.format = FLAC;
	track.path = path;
	track.name = name.c_str();
	drflac_int16* pSampleData = drflac_open_file_and_read_pcm_frames_s16(path, &track.channels, &track.sampleRate, &totalPCMFrameCount, nullptr);
	track.SetPCMFrameCount(totalPCMFrameCount);

	if (pSampleData == NULL)
	{
		std::cerr << "failed to load audio file" << std::endl;
		return Track();
	}

	track.bits = 16;

	if (track.getTotalSamples() > drflac_uint64(std::numeric_limits<size_t>::max()))
	{
		std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
		return Track();
	}

	track.pcmData.resize(size_t(track.getTotalSamples()));
	std::memcpy(track.pcmData.data(), pSampleData, track.pcmData.size() * /*twobytes_in_s15*/2);
	drflac_free(pSampleData, nullptr);

	ALuint buffer;
	alec(alGenBuffers(1, &buffer));
	alec(alBufferData(buffer, track.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, track.pcmData.data(), track.pcmData.size() * 2 /*two bytes per sample*/, track.sampleRate));
	
	ALint bufferSize;
	alec(alGetBufferi(buffer, AL_SIZE, &bufferSize));
	track.duration = bufferSize / (track.sampleRate * track.channels * 2); //(2 bytes == 16 Bits)
	
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

