#pragma once
#include "Module.h"
#include "Globals.h"

#include "External/SDL\include\SDL.h"

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

class ModuleRender : public Module
{
public:
	ModuleRender(Application* app, bool start_enabled = true);
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();

	bool DrawTexture(SDL_Texture* texture, int x, int y, SDL_Rect* section = NULL, float speed = 1.0f, double angle = 0, int pivot_x = INT_MAX, int pivot_y = INT_MAX);
	bool DrawQuad(const SDL_Rect& rect, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool filled = true, bool use_camera = true);
	bool DrawLine(int x1, int y1, int x2, int y2, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true);
	bool DrawCircle(int x1, int y1, int redius, Uint8 r, Uint8 g, Uint8 b, Uint8 a = 255, bool use_camera = true);

public:
	SDL_Renderer* renderer;
	SDL_Rect camera;
	ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};