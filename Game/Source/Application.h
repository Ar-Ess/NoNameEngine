#pragma once

#include "Globals.h"
#include "External/imgui/imgui.h"
#include "External/json/json.h"
#include "External/rapidjson/document.h"
#include "External/rapidjson/writer.h"
#include "External/rapidjson/stringbuffer.h"
#include "External/parson/Parson.h"
#include <list>
#include <string>

using namespace std;
using namespace rapidjson;

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
	bool Load(string fileName);
	// Do not add the extension when writing the file name
	bool Save(string fileName);

	SDL_Window* mainWindow = nullptr;

private:

	void AddModule(Module* mod);

private: //Variables

	bool loadRequest = false;
	bool saveRequest = false;
	string loadFileName;
	string saveFileName;

};