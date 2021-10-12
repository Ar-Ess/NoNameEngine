#pragma once

#include "p2List.h"
#include "Globals.h"
#include "Module.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModulePlayer.h"
#include "ModuleSceneIntro.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "ModulePhysics3D.h"

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

class Module;
class ModuleWindow;
class ModuleTextures;
class ModuleInput;
class ModuleAudio;
class ModulePlayer;
class ModuleCamera;
class ModuleRenderer3D;
class ModulePhysics;
class ModuleSceneIntro;

class Application
{
public:
	ModuleWindow* window;
	ModuleTextures* textures;
	ModuleInput* input;
	ModuleRenderer3D* render;
	ModuleCamera3D* camera;
	ModulePhysics3D* physics;
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

private:

	void AddModule(Module* mod);
};