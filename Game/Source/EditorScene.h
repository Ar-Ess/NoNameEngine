#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "Globals.h"
#include <string>

#include "../Source/External/ImGui/imgui.h"
#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl2.h"
#include "../Source/External/ImGui/imgui_stdlib.h"

#include "Primitive.h"

#include <stdio.h>
#include "External/SDL/include/SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "External/SDL/include/SDL_opengl.h"
#endif

#define YELLOW {1.0f, 1.0f, 0.0f, 1.0f}

class EditorScene
{
public:
	EditorScene(Application* app);
	~EditorScene();

	bool Start();
	bool Update();

	bool CleanUp();

private: // Functions
	bool DrawMenuBar();

	bool ShowAboutWindow(bool open);
	bool ShowOutputWindow(bool open);
	bool ShowConfigWindow(bool open);

	// Instantly browse on internet the link written as an argument
	void LinkBrowser(string link)
	{
		ShellExecuteA(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	// Input from 0 (smallest spacing) to whatever you need
	void AddSpacing(unsigned int spaces = 1)
	{
		short int plus = 0;
		if (spaces == 0) plus = 1;
		for (int i = 0; i < spaces * 2 + plus; i++) ImGui::Spacing();
	}

	// Input from 1 to whatever you need
	void AddSeparator(unsigned int separator = 1)
	{
		if (separator == 0) return;
		for (int i = 0; i < separator; i++) ImGui::Separator();
	}

public: // Variables
	bool demoWindow = false;
	bool outputWindow = false;
	bool configWindow = false;

	bool aboutPopup = false;

	float brightLevel = 1.0f;
	bool wFullScreen = false;
	bool wFullDesktop = false;
	bool wResizable = false;
	bool wBorderless = false;
	bool wVSync = false;
	bool wKeepProportion = false;

	Point wSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
	int wSizeProportion = 100;

	std::string projectName = "New Project";
	std::string teamName = "Team Name";

private: // Variables

	ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	Application* app = nullptr;
	float framerate = 0.0f;
	float milliseconds = 0.0f;

	Plane p0 = { 0, 1, 0, 0};
};
