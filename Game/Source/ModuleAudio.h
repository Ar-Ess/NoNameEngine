#pragma once
#include <iostream>
#include "Globals.h"
#define DR_WAV_IMPLEMENTATION
#include "External/OpenAL/include/AL/al.h"
#include "External/OpenAL/include/AL/alc.h"
#include "External/OpenAL/include/AL/alext.h"
#include "External/DrWav/dr_wav.h"
#include <vector>

class ModuleAudio
{
public:

	ModuleAudio();
	~ModuleAudio();

	void InitAudio();
	void CreateListener();
};