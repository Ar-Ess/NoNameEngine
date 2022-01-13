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
	device = 0;
}

AudioSystem::~AudioSystem()
{

}

void AudioSystem::InitAudio()
{
	const ALCchar* defaultDeviceStr = alcGetString(nullptr, ALC_DEFAULT_DEVICE_SPECIFIER);
	device = alcOpenDevice(defaultDeviceStr);
	if (!device)
	{
		/*LOG("Failed to get the default device for OpenAL");*/

	}
	/*LOG("OpenAL Device", alcGetString(device, ALC_DEVICE_SPECIFIER));*/

	context = alcCreateContext(device, nullptr);

	if (!alcMakeContextCurrent(context))
	{
		/*LOG("Failed to make the OpenAL context the current context");*/
	}
}

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

ALuint* AudioSystem::CreateBuffers()
{
	drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16("Assets/Audiotest/wav_mono_16bit_44100.wav", &monoData.channels, &monoData.sampleRate, &monoData.totalPCMFrameCount, nullptr);
	if (pSampleData == NULL) {
		std::cerr << "failed to load audio file" << std::endl;
		drwav_free(pSampleData, nullptr); //todo use raii to clean this up
		return 0;
	}
	if (monoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
	{
		std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
		drwav_free(pSampleData, nullptr);
		return 0;
	}
	monoData.pcmData.resize(size_t(monoData.getTotalSamples()));
	std::memcpy(monoData.pcmData.data(), pSampleData, monoData.pcmData.size() * /*twobytes_in_s16*/2);
	drwav_free(pSampleData, nullptr);

	ALuint monoSoundBuffer;
	alec(alGenBuffers(1, &monoSoundBuffer));
	alec(alBufferData(monoSoundBuffer, monoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, monoData.pcmData.data(), monoData.pcmData.size() * 2, monoData.sampleRate));

	return &monoSoundBuffer;
}

ALuint* AudioSystem::LoadStereo()
{
	ReadWav stereoData;
	{
		drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16("Assets/Audiotest/wav_mono_16bit_44100.wav", &stereoData.channels, &stereoData.sampleRate, &stereoData.totalPCMFrameCount, nullptr);
		if (pSampleData == NULL) {
			std::cerr << "failed to load audio file" << std::endl;
			return 0;
		}
		if (stereoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max()))
		{
			std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
			return 0;
		}
		stereoData.pcmData.resize(size_t(stereoData.getTotalSamples()));
		std::memcpy(stereoData.pcmData.data(), pSampleData, stereoData.pcmData.size() * /*twobytes_in_s15*/2);
		drwav_free(pSampleData, nullptr);
	}

	ALuint stereoSoundBuffer;
	alec(alGenBuffers(1, &stereoSoundBuffer));
	alec(alBufferData(stereoSoundBuffer, stereoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, stereoData.pcmData.data(), stereoData.pcmData.size() * 2 /*two bytes per sample*/, stereoData.sampleRate));

	return &stereoSoundBuffer;
}



ALuint* AudioSystem::CreateMonoSource(ALuint* buffer)
{
	ALuint monoSource;
	alec(alGenSources(1, &monoSource));
	alec(alSource3f(monoSource, AL_POSITION, 1.0f, 0.0f, 0.0f));
	alec(alSource3f(monoSource, AL_VELOCITY, 0.f, 0.f, 0.f));
	alec(alSourcef(monoSource, AL_PITCH, 1.f));
	alec(alSourcef(monoSource, AL_GAIN, 1.f));
	alec(alSourcei(monoSource, AL_LOOPING, AL_FALSE));
	alec(alSourcei(monoSource, AL_BUFFER, *buffer));

	return &monoSource;
}

ALuint* AudioSystem::CreateStereoSource(ALuint* buffer)
{
	ALuint stereoSource;
	alec(alGenSources(1, &stereoSource));
	//alec(alSource3f(stereoSource, AL_POSITION, 0.f, 0.f, 1.f)); //NOTE: this does not work like mono sound positions!
	//alec(alSource3f(stereoSource, AL_VELOCITY, 0.f, 0.f, 0.f)); 
	alec(alSourcef(stereoSource, AL_PITCH, 1.f));
	alec(alSourcef(stereoSource, AL_GAIN, 1.f));
	alec(alSourcei(stereoSource, AL_LOOPING, AL_FALSE));
	alec(alSourcei(stereoSource, AL_BUFFER, *buffer));

	return &stereoSource;
}

void AudioSystem::PlayMonoSound(ALuint* monoSource)
{
	alec(alSourcePlay(*monoSource));
	ALint sourceState;
	alec(alGetSourcei(*monoSource, AL_SOURCE_STATE, &sourceState));
	while (sourceState == AL_PLAYING)
	{
		alec(alGetSourcei(*monoSource, AL_SOURCE_STATE, &sourceState))
	}

}

void AudioSystem::PlayStereoSound(ALuint* stereoSource)
{
	alec(alSourcePlay(*stereoSource));
	ALint sourceState;
	alec(alGetSourcei(*stereoSource, AL_SOURCE_STATE, &sourceState));
	while (sourceState == AL_PLAYING)
	{
		//basically loop until we're done playing the mono sound source
		alec(alGetSourcei(*stereoSource, AL_SOURCE_STATE, &sourceState));
	}
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

