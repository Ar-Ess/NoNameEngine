#pragma once

#include "Globals.h"

#include "External/glew/glew.h"
#include "External/SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

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
class ModuleInput;
class ModuleScene;
class ModuleCamera3D;
class ModuleRenderer3D;

class Application
{
public:
	ModuleWindow* window = nullptr;
	ModuleInput* input = nullptr;
	ModuleScene* scene = nullptr;
	ModuleCamera3D* camera = nullptr;
	ModuleRenderer3D* render = nullptr;

private:

	list<Module*> moduleList = {};

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
	bool SaveRestartPropierties();
	bool LoadRestartPropierties();
	uint32 GetFrameCount() const
	{
		return frameCount;
	}
	float GetDeltaTime() const
	{
		return dt;
	}

private:

	void AddModule(Module* mod);
	void ProfilerLogic();

private: // JSON Functions

	void ArrayAppendVector(JSON_Array* arr, vec3 vec)
	{
		for (int i = 0; i < 3; i++)
			json_array_append_number(arr, vec[i]);
	}

	void ArrayAppendVector(JSON_Array* arr, Point3D vec)
	{
		for (int i = 0; i < 3; i++)
			json_array_append_number(arr, vec[i]);
	}

	void ArrayRetrieveVector(JSON_Array* arr, vec3* vec, int offSet)
	{
		double array[3] = {};
		for (int i = 0; i < 3; i++)
			array[i] = json_array_get_number(arr, (i + (3 * offSet)));

		vec->x = array[0];
		vec->y = array[1];
		vec->z = array[2];
	}

	void ArrayRetrieveVector(JSON_Array* arr, Point3D* vec, int offSet)
	{
		double array[3] = {};
		for (int i = 0; i < 3; i++) array[i] = json_array_get_number(arr, (i + (3 * offSet)));

		vec->x = array[0];
		vec->y = array[1];
		vec->z = array[2];
	}

public: // Variables

	std::vector<float> fpsLog = {};
	int frameBarLimit = 80;
	std::vector<float> msLog = {};
	int msBarLimit = 80;
	int fps = 60;

private: // Variables

	bool loadRequest = false;
	bool saveRequest = false;
	string fileName = {};
	FileContent fileContent = {};

	// L07: DONE 4: Calculate some timing measures
	// required variables are provided:
	PerfTimer ptimer = {};
	uint64 frameCount = 0;

	Timer startupTime = {};
	Timer frameTime = {};
	Timer lastSecFrameTime = {};
	uint32 lastSecFrameCount = 0;
	uint32 prevLastSecFrameCount = 0;
	float dt = 0.0f;

	uint32 frameDelay = 1000 / 60;

	int	cappedMs = -1;
};