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

enum class WindowSettings
{
	FULL_SCREEN,
	FULL_DESKTOP,
	RESIZABLE,
	BORDERLESS,
	V_SYNC,
	KEEP_PROPORTION
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
	float GetBrightness()
	{
		return editor->brightLevel;
	}
	void SetBrightness(float brightness)
	{
		editor->brightLevel = brightness;
	}
	bool GetWindowSettings(WindowSettings wS)
	{
		bool state = false;

		switch (wS)
		{
		case WindowSettings::FULL_SCREEN: state = editor->wFullScreen; break;
		case WindowSettings::FULL_DESKTOP: state = editor->wFullDesktop; break;
		case WindowSettings::RESIZABLE: state = editor->wResizable; break;
		case WindowSettings::BORDERLESS: state = editor->wBorderless; break;
		case WindowSettings::V_SYNC: state = editor->wVSync; break;
		case WindowSettings::KEEP_PROPORTION: state = editor->wKeepProportion; break;
		}

		return state;
	}
	void SetWindowSettings(WindowSettings wS, bool state)
	{
		switch (wS)
		{
		case WindowSettings::FULL_SCREEN: editor->wFullScreen = state; break;
		case WindowSettings::FULL_DESKTOP: editor->wFullDesktop = state; break;
		case WindowSettings::RESIZABLE: editor->wResizable = state; break;
		case WindowSettings::BORDERLESS: editor->wBorderless = state; break;
		case WindowSettings::V_SYNC: editor->wVSync = state; break;
		case WindowSettings::KEEP_PROPORTION: editor->wKeepProportion = state; break;
		}
	}
	Point GetWinDimensions()
	{
		return editor->wSize;
	}
	void SetWinDimensions(Point size)
	{
		editor->wSize = size;
	}
	int GetWinDimensionProportion()
	{
		return editor->wSizeProportion;
	}
	void SetWinDimensionProportion(int prop)
	{
		editor->wSizeProportion = prop;
	}


private:
	Scenes scene = Scenes::NO_SCENE;
	Scenes prevScene = Scenes::NO_SCENE;
	Scenes changeScene = Scenes::NO_SCENE;

	bool sceneChangeRequest = false;

	EditorScene* editor = nullptr;
};
