#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "Globals.h"
#include <string>

#include "../Source/External/ImGui/imgui.h"
#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl3.h"
#include "../Source/External/ImGui/imgui_stdlib.h"

#include "Shapes3D.h"

#include <stdio.h>
#include "External/SDL/include/SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "External/SDL/include/SDL_opengl.h"
#endif

#define YELLOW {1.0f, 1.0f, 0.0f, 1.0f}

class Plane3D;
class Cube3D;
class Cylinder3D;
class Pyramid3D;

class EditorScene
{
public:
	EditorScene(Application* app, vector<Shape3D*>* shapes);
	~EditorScene();

	bool Start();
	bool Update();

	bool CleanUp();

private: // Functions
	bool DrawMenuBar();

	bool ShowAboutWindow(bool open);
	bool ShowOutputWindow(bool open);
	bool ShowConfigWindow(bool open);
	bool ShowHierarchyWindow(bool open);

	bool ShortCuts();

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

	// Create a default primitive
	void CreatePrimitive(ShapeType sT);

	// Push Back of a Shape3D
	void PushShape3D(Shape3D* s3D);

	// Duplicates selected shape
	void DuplicateSelecShape();

	// Pop First of a Shape3D
	void PopShape();

	// Pop All Shapes on scene
	void PopAllShapes();

public: // Variables
	bool demoWindow = false;
	bool outputWindow = false;
	bool configWindow = false;
	bool hierarchyWindow = true;

	bool aboutPopup = false;

	float brightLevel = 1.0f;
	bool wFullScreen = false;
	bool wFullDesktop = false;
	bool wResizable = false;
	bool wBorderless = false;
	bool wVSync = false;
	bool wKeepProportion = false;

	// Geometry View
	bool depthTest = false;
	bool cullFace = false;
	bool lighting = false;
	bool colorMaterial = false;
	bool texture2D = false;

	// Geometry Info
	bool edges = true;
	bool normals = false;
	bool solid = true;

	// Information for Cam
	bool onWindow = false;


	Point wSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
	int wSizeProportion = 100;

	std::string projectName = "New Project";
	std::string teamName = "Team Name";

private: // Variables

	ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	Application* app = nullptr;
	float framerate = 0.0f;
	float milliseconds = 0.0f;
	vector<Shape3D*>* shapes = nullptr;
};
