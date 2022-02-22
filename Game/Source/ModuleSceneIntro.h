#pragma once
#include "Module.h"
#include "Globals.h"
#include "EditorScene.h"
#include "Timer.h"
#include <string>
#include "Shapes3D.h"
#include <vector>
#include "ModuleRenderer3D.h"
#include "ModuleInput.h"
#include "AssetsManager.h"
#include "ImportManager.h"
#include "AudioSystem.h"
#include "Model.h"

class Shape3D;
class Model;
class ModuleInput;

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
	HIERARCHY_W,
	ABOUT_W
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

enum class GeometryView;

enum class GeometryInfo
{
	EDGES,
	NORMALS,
	SOLID
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

private: // Private functions
	friend class ModuleInput;

	void LoadDropModel(const char* path)
	{
		Model* m = new Model({ 0, 0, 0 });
		m->LoadModel(path);
		shapes.push_back(m);
	}

	void LoadDropTexture(const char* path)
	{
		bool setted = false;
		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			if (shapes[i]->GetShapeType() == ShapeType::MODEL3D && shapes[i]->selected)
			{
				Model* m = (Model*)shapes[i];
				m->LoadTexture(path);
				setted = true;
			}
		}

		if (!setted)
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_INFORMATION,
				"Texture Error",
				"\nAny Model3D selected",
				app->window->mainWindow
			);
		}
	}

public: // Getters & Setters

	AudioSystem* GetAudioSystem()
	{
		return editor->GetAudioSystem();
	}

	bool GetWindowState(Windows w)
	{
		bool state = false;

		switch (w)
		{
		case Windows::OUTPUT_W: state = editor->outputWindow; break;
		case Windows::CONFIG_W: state = editor->configWindow; break;
		case Windows::DEMO_W: state = editor->demoWindow; break;
		case Windows::ABOUT_W: state = editor->aboutPopup; break;
		case Windows::HIERARCHY_W: state = editor->hierarchyWindow; break;
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
		case Windows::HIERARCHY_W: editor->hierarchyWindow = state; break;
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
		app->window->SetWinBrightness(brightness);
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
		case WindowSettings::FULL_SCREEN: editor->wFullScreen = state; app->window->SetWinFullScreen(state); break;
		case WindowSettings::FULL_DESKTOP: editor->wFullDesktop = state; app->window->SetWinDFullScreen(state); break;
		case WindowSettings::RESIZABLE: editor->wResizable = state; break;
		case WindowSettings::BORDERLESS: editor->wBorderless = state; app->window->SetWinBorders(state); break;
		case WindowSettings::V_SYNC: editor->wVSync = state; app->render->SetVSync(state); break;
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
		app->window->SetWinSize(int(size.x), int(size.y));
	}
	int GetWinDimensionProportion()
	{
		return editor->wSizeProportion;
	}
	void SetWinDimensionProportion(int prop)
	{
		editor->wSizeProportion = prop;
		app->window->SetWinSize(int(floor(SCREEN_WIDTH * prop / 100)), int(floor(SCREEN_HEIGHT * prop / 100)));
	}
	bool GetGeometryView(GeometryView gV)
	{
		bool ret = false;

		switch (gV)
		{
		case GeometryView::DEPTH_TEST: ret = editor->depthTest;
		case GeometryView::CULL_FACE: ret = editor->cullFace;
		case GeometryView::LIGHTING: ret = editor->lighting;
		case GeometryView::COLOR_MATERIAL: ret = editor->colorMaterial;
		case GeometryView::TEXTURE_2D: ret = editor->texture2D;
		}

		return ret;
	}
	void SetGeometryView(GeometryView gV, bool state)
	{
		switch (gV)
		{
		case GeometryView::DEPTH_TEST: editor->depthTest = state;
		case GeometryView::CULL_FACE: editor->cullFace = state;
		case GeometryView::LIGHTING: editor->lighting = state;
		case GeometryView::COLOR_MATERIAL: editor->colorMaterial = state;
		case GeometryView::TEXTURE_2D: editor->texture2D = state;
		}

		app->render->ToggleGeometryView(gV, state);
	}
	void SetGeometryInfo(GeometryInfo gI)
	{
		bool setted = false;
		for (unsigned int i = 0; i < shapes.size(); i++)
		{
			if (shapes[i]->selected)
			{
				switch (gI)
				{
				case GeometryInfo::EDGES: shapes[i]->edges = editor->edges; break;
				case GeometryInfo::NORMALS: shapes[i]->normals = editor->normals; break;
				case GeometryInfo::SOLID: shapes[i]->solid = editor->solid; break;
				}
				setted = true;
			}
		}

		if (!setted)
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_INFORMATION,
				"Shape Error",
				"\nAny Shape selected",
				app->window->mainWindow
			);
		}
	}
	bool GetOnWindow()
	{
		return editor->onWindow;
	}

public:
	vector<Shape3D*> shapes = {};
	Timer gameTimer;

private:
	Scenes scene = Scenes::EDITOR;
	Scenes prevScene = Scenes::NO_SCENE;
	Scenes changeScene = Scenes::NO_SCENE;

	bool sceneChangeRequest = false;

	EditorScene* editor = nullptr;
};
