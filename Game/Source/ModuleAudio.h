#pragma once
#include "Globals.h"
#define DR_WAV_IMPLEMENTATION
#include "External/DrWav/dr_wav.h"
#include "External/OpenAL/include/AL/al.h"
#include "External/OpenAL/include/AL/alc.h"
#include <vector>

class ModuleAudio
{
public:

	ModuleAudio();
	~ModuleAudio();

public:

	void InitAudio();
	/*void CreateListener();*/

};