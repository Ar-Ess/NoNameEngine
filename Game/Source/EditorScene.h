#pragma once
#include "Application.h"
#include "Globals.h"

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

class EditorScene
{
public:
	EditorScene(Application* app);
	~EditorScene();

	bool Start();
	bool Update();

	bool CleanUp();

	bool exampleWindow = false;

private:
	//bool exampleWindow = false;
	ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	Application* app = nullptr;

public:
};
