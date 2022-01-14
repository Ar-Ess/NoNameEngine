#include "ModuleSound.h"

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

ModuleSound::ModuleSound(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleSound::~ModuleSound()
{

}

bool ModuleSound::Init()
{
	bool ret = true;
	InitAudio();
	return ret;
}

bool ModuleSound::Start()
{
	bool ret = true;
	CreateListener();

	monoBuffer = CreateBuffers();
	/*stereoBuffer = LoadStereo();*/
	monoSource = CreateMonoSource(monoBuffer);
	/*stereoSource = CreateStereoSource(stereoBuffer);*/
	return ret;
}

update_status ModuleSound::Update()
{
	/*PlayMonoSound(monoSource);*/
	/*PlayStereoSound(stereoSource);*/
	return UPDATE_CONTINUE;
}

bool ModuleSound::CleanUp()
{
	bool ret = true;
	return ret;
}

void ModuleSound::InitAudio()
{
	
	device = alcOpenDevice(NULL);
	if (!device)
	{
		LOG("Failed to get the default device for OpenAL");

	}
	LOG("OpenAL Device", alcGetString(device, ALC_DEVICE_SPECIFIER));

	ListAudioDevices(alcGetString(NULL, ALC_DEVICE_SPECIFIER));
	ALboolean enumeration;
	enumeration = alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT");
	if (enumeration == AL_FALSE)
	{
		LOG("Enumeration not supported");
	}
	context = alcCreateContext(device, nullptr);

	ALboolean bEAX = alIsExtensionPresent("EAX2.0");
	if (bEAX == AL_FALSE)
	{
		LOG("EAX not supported");
	}

	if (!alcMakeContextCurrent(context))
	{
		LOG("Failed to make the OpenAL context the current context");
	}
}

void ModuleSound::ListAudioDevices(const ALCchar* devices)
{
	const ALCchar* device = devices, * next = devices + 1;
	size_t len = 0;

	LOG("Devices list: ");
	fprintf(stdout, "----------\n");
	while (device && *device != '\0' && next && *next != '\0') {
		LOG("%s", device);
		len = strlen(device);
		device += (len + 1);
		next += (len + 2);
	}
}

void ModuleSound::CreateListener()
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

ALuint ModuleSound::CreateBuffers()
{
	drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16("Assets/Audiotest/wav_mono_16bit_44100.wav", &monoData.channels, &monoData.sampleRate, &monoData.totalPCMFrameCount, nullptr);
	if (pSampleData == NULL) {
		LOG("failed to load audio file");
		drwav_free(pSampleData, nullptr); //todo use raii to clean this up
		return 0;
	}
	if (monoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max))
	{
		LOG("too much data in file for 32bit addressed vector");
		drwav_free(pSampleData, nullptr);
		return 0;
	}
	monoData.pcmData.resize(size_t(monoData.getTotalSamples()));
	std::memcpy(monoData.pcmData.data(), pSampleData, monoData.pcmData.size() * /*twobytes_in_s16*/2);
	drwav_free(pSampleData, nullptr);

	ALuint monoSoundBuffer;
	alec(alGenBuffers(1, &monoSoundBuffer));
	alec(alBufferData(monoSoundBuffer, monoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, monoData.pcmData.data(), monoData.pcmData.size() * 2, monoData.sampleRate));

	return monoSoundBuffer;
}

ALuint ModuleSound::LoadStereo()
{
	ReadWav stereoData;
	{
		drwav_int16* pSampleData = drwav_open_file_and_read_pcm_frames_s16("Assets/AudioTest/wav_stereo_16bit_44100.wav", &stereoData.channels, &stereoData.sampleRate, &stereoData.totalPCMFrameCount, nullptr);
		if (pSampleData == NULL) {
			std::cerr << "failed to load audio file" << std::endl;
			return 0;
		}
		if (stereoData.getTotalSamples() > drwav_uint64(std::numeric_limits<size_t>::max))
		{
			std::cerr << "too much data in file for 32bit addressed vector" << std::endl;
			return 0;
		}
		stereoData.pcmData.resize(size_t(stereoData.getTotalSamples()));
		std::memcpy(stereoData.pcmData.data(), pSampleData, stereoData.pcmData.size() * /*twobytes_in_s15*/2);
		drwav_free(pSampleData, nullptr);
	}

	ALuint stereoSoundBuffer = 1;
	alec(alGenBuffers(1, &stereoSoundBuffer));
	alec(alBufferData(stereoSoundBuffer, stereoData.channels > 1 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, stereoData.pcmData.data(), stereoData.pcmData.size() * 2 /*two bytes per sample*/, stereoData.sampleRate));

	return stereoSoundBuffer;
}



ALuint ModuleSound::CreateMonoSource(ALuint buffer)
{
	ALuint monoSource;
	alec(alGenSources(1, &monoSource));
	alec(alSource3f(monoSource, AL_POSITION, 1.0f, 0.0f, 0.0f));
	alec(alSource3f(monoSource, AL_VELOCITY, 0.f, 0.f, 0.f));
	alec(alSourcef(monoSource, AL_PITCH, 1.f));
	alec(alSourcef(monoSource, AL_GAIN, 1.f));
	alec(alSourcei(monoSource, AL_LOOPING, AL_FALSE));
	alec(alSourcei(monoSource, AL_BUFFER, buffer));

	return monoSource;
}

ALuint ModuleSound::CreateStereoSource(ALuint buffer)
{
	ALuint stereoSource;
	alec(alGenSources(1, &stereoSource));
	alec(alSourcef(stereoSource, AL_PITCH, 1.f));
	alec(alSourcef(stereoSource, AL_GAIN, 1.f));
	alec(alSourcei(stereoSource, AL_LOOPING, AL_FALSE));
	alec(alSourcei(stereoSource, AL_BUFFER, buffer));

	return stereoSource;
}

void ModuleSound::PlayMonoSound(ALuint monoSource)
{
	alec(alSourcePlay(monoSource));
	ALint sourceState;
	alec(alGetSourcei(monoSource, AL_SOURCE_STATE, &sourceState));
	/*while (sourceState == AL_PLAYING)
	{
		alec(alGetSourcei(monoSource, AL_SOURCE_STATE, &sourceState))
	}*/

}

void ModuleSound::PlayStereoSound(ALuint stereoSource)
{
	alec(alSourcePlay(stereoSource));
	ALint sourceState;
	alec(alGetSourcei(stereoSource, AL_SOURCE_STATE, &sourceState));
	while (sourceState == AL_PLAYING)
	{
		//basically loop until we're done playing the mono sound source
		alec(alGetSourcei(stereoSource, AL_SOURCE_STATE, &sourceState));
	}
}

void ModuleSound::CleanUp(ALuint monoBuffer, ALuint stereoBuffer, ALuint monoSource, ALuint stereoSource)
{
	alec(alDeleteSources(1, &monoSource));
	alec(alDeleteSources(1, &stereoSource));
	alec(alDeleteBuffers(1, &monoBuffer));
	alec(alDeleteBuffers(1, &stereoBuffer));
	alcMakeContextCurrent(nullptr);
	alcDestroyContext(context);
	alcCloseDevice(device);
}

void ModuleSound::CleanUpSource(ALuint source)
{
	alec(alDeleteSources(1, &source));
}

void ModuleSound::CleanUpBuffer(ALuint buffer)
{
	alec(alDeleteSources(1, &buffer));
}


