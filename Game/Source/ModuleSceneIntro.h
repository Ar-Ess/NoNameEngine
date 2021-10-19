#pragma once
#include "Module.h"
#include "Globals.h"
#include "EditorScene.h"

#include "../Source/External/ImGui/imgui.h"
#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl2.h"

#include <stdio.h>
#include "External/SDL/include/SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "External/SDL/include/SDL_opengl.h"
#endif

enum class Scenes
{
	NO_SCENE = 0,
	EDITOR
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

	bool exampleWindow = false;

private:
	Scenes scene = Scenes::NO_SCENE;
	Scenes prevScene = Scenes::NO_SCENE;
	Scenes changeScene = Scenes::NO_SCENE;

	bool sceneChangeRequest = false;

	EditorScene* editor = nullptr;


public:
};
