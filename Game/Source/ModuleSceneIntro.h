#pragma once
#include "Module.h"
#include "Globals.h"
#include "EditorScene.h"
#include <string>

enum class Scenes
{
	NO_SCENE = 0,
	EDITOR
};

enum class Windows
{
	OUTPUT_W,
	DEMO_W,
	CONFIG_W,
};

class ModuleScene : public Module
{
public:
	ModuleScene(Application* app, bool start_enabled = true);
	~ModuleScene();

	bool Start();
	update_status Update();

	// Scene Manager
	bool UpdateEditor();
	bool SetScene(Scenes newScene);
	bool CleanUpScene(Scenes scene);

	bool CleanUp();

public: // Getters & Setters

	bool GetWindowState(Windows w)
	{
		bool state = false;

		switch (w)
		{
		case Windows::OUTPUT_W: state = editor->outputWindow; break;
		case Windows::CONFIG_W: state = editor->configWindow; break;
		case Windows::DEMO_W: state = editor->demoWindow; break;
		}

		return state;
	}
	void SetWindowState(Windows w, bool state)
	{
		switch (w)
		{
		case Windows::OUTPUT_W: editor->outputWindow = state; break;
		case Windows::CONFIG_W: editor->configWindow = state; break;
		case Windows::DEMO_W: editor->demoWindow = state; break;
		}
	}
	std::string GetProjectName()
	{
		return editor->projectName;
	}
	void SetProjectName(const char* name)
	{
		editor->projectName.clear();
		editor->projectName = name;
	}
	std::string GetTeamName()
	{
		return editor->teamName;
	}
	void SetTeamName(const char* name)
	{
		editor->teamName.clear();
		editor->teamName = name;
	}

private:
	Scenes scene = Scenes::NO_SCENE;
	Scenes prevScene = Scenes::NO_SCENE;
	Scenes changeScene = Scenes::NO_SCENE;

	bool sceneChangeRequest = false;

	EditorScene* editor = nullptr;
};
