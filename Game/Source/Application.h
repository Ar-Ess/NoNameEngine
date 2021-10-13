#pragma once

#include "p2List.h"
#include "Globals.h"
#include "External/imgui/imgui.h"

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModulePlayer;
class ModuleSceneIntro;
class SDL_Window;

class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleAudio* audio;
	ModulePlayer* player;
	ModuleSceneIntro* scene_intro;

private:

	p2List<Module*> list_modules;

public:

	Application();
	~Application();

	bool Init();
	bool InitImGui();
	update_status Update();
	bool CleanUp();
	void CleanImGui();

	SDL_Window* mainWindow = nullptr;

	bool GetQuitState()
	{
		return quit;
	}

private:

	void AddModule(Module* mod);

private: //Variables
	bool quit = false;
};