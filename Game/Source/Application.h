#pragma once

#include "Globals.h"
#include "External/imgui/imgui.h"
#include <list>
using namespace std;

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleSceneIntro;
class SDL_Window;

class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleSceneIntro* scene_intro;

private:

	list<Module*> moduleList;

public:

	Application();
	~Application();

	bool Init();
	bool InitImGui();
	update_status Update();
	bool CleanUp();
	void CleanImGui();

	SDL_Window* mainWindow = nullptr;

private:

	void AddModule(Module* mod);

};