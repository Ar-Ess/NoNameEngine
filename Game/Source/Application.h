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

enum class FileContent
{
	PROJECT,
	CONFIG_PREFERENCES
};

class Module;
class ModuleRender;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleScene;
class SDL_Window;

class Application
{
public:
	ModuleRender* renderer;
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleScene* scene;

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
	bool Load(string fileName, FileContent fileType);
	// Do not add the extension when writing the file name
	bool Save(string fileName, FileContent fileType);

	SDL_Window* mainWindow = nullptr;

private:

	void AddModule(Module* mod);

private: //Variables

	bool loadRequest = false;
	bool saveRequest = false;
	string fileName;
	FileContent fileContent;

};