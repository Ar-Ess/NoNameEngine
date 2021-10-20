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
#include <vector>
#include "PerfTimer.h"
#include "Timer.h"
#include "Defs.h"

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
	uint32 GetFrameCount() const
	{
		return frameCount;
	}
	float GetDeltaTime() const
	{
		return dt;
	}

	SDL_Window* mainWindow = nullptr;

private:

	void AddModule(Module* mod);
	void ProfilerLogic();

public: // Variables

	std::vector<float> fpsLog;
	int frameBarLimit = 80;
	std::vector<float> msLog;
	int msBarLimit = 80;
	int fps = 60;

private: // Variables

	bool loadRequest = false;
	bool saveRequest = false;
	string fileName;
	FileContent fileContent;

	// L07: DONE 4: Calculate some timing measures
	// required variables are provided:
	PerfTimer ptimer;
	uint64 frameCount = 0;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	float dt = 0.0f;

	uint32 frameDelay = 1000 / 60;

	int	cappedMs = -1;
};