#pragma once
#include "Module.h"
#include "Globals.h"
#include "External/SDL\include\SDL.h"
#include <list>
using namespace std;

class ModuleTextures : public Module
{
public:
	ModuleTextures(Application* app, bool start_enabled = true);
	~ModuleTextures();

	bool Init();
	bool CleanUp();

public:
	list<SDL_Texture*> textures;
};