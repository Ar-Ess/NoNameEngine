#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"

//#include "Primitive.h"

#include <stdio.h>
#include <iostream>
#include <fstream>
#include "External/SDL/include/SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "External/SDL/include/SDL_opengl.h"
#endif

#include "Application.h"

Application::Application()
{
	PERF_START(ptimer);

	render = new ModuleRenderer3D(this);
	window = new ModuleWindow(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);
	camera = new ModuleCamera3D(this);

	// Main Modules
	AddModule(window);
	AddModule(camera);
	AddModule(input);
	
	// Scenes
	AddModule(scene);

	// Render
	AddModule(render);

	PERF_PEEK(ptimer);
}

Application::~Application()
{
	for (list<Module*>::iterator item = moduleList.end(); item != moduleList.begin(); --item) delete item._Ptr;
}

bool Application::Init()
{
	bool ret = true;

	PERF_START(ptimer);

	InitImGui();

	list<Module*>::iterator item;

	for (item = moduleList.begin(); item != moduleList.end(); ++item) 
		ret = item._Ptr->_Myval->Init();

	for (item = moduleList.begin(); item != moduleList.end(); ++item) 
		if (item._Ptr->_Myval->IsEnabled()) ret = item._Ptr->_Myval->Start();

	PERF_PEEK(ptimer);

	return ret;
}

bool Application::InitImGui()
{
	bool ret = true;

	PERF_START(ptimer);

	// Setup SDL
	// (Some versions of SDL before <2.0.10 appears to have performance/stalling issues on a minority of Windows systems,
	// depending on whether SDL_INIT_GAMECONTROLLER is enabled or disabled.. updating to latest version of SDL is recommended!)
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
	{
		printf("Error: %s\n", SDL_GetError());
		return -1;
	}

	// GL 3.0 + GLSL 130
	const char* glsl_version = "#version 130";
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);

	PERF_PEEK(ptimer);

	return ret;
}

void Application::CleanImGui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

update_status Application::Update()
{
	bool call = false;
	update_status ret = UPDATE_CONTINUE;

	list<Module*>::iterator item;

	// Prepare Update
	frameCount++;
	lastSecFrameCount++;

	dt = frameTime.ReadSec();
	frameTime.Start();

	//PreUpdate
	for (item = moduleList.begin(); item != moduleList.end() && ret == UPDATE_CONTINUE; ++item) 
		if (item._Ptr->_Myval->IsEnabled()) ret = item._Ptr->_Myval->PreUpdate();

	//Update
	for (item = moduleList.begin(); item != moduleList.end() && ret == UPDATE_CONTINUE; ++item)
		if (item._Ptr->_Myval->IsEnabled()) ret = item._Ptr->_Myval->Update();

	//PostUpdate
	for (item = moduleList.begin(); item != moduleList.end() && ret == UPDATE_CONTINUE; ++item)
		if (item._Ptr->_Myval->IsEnabled()) ret = item._Ptr->_Myval->PostUpdate();

	
	if (loadRequest) call = Load(fileName, fileContent);
	if (saveRequest && !call) Save(fileName, fileContent);

	ProfilerLogic();

	return ret;
}

bool Application::CleanUp()
{
	bool ret = true;

	window->SetWinBrightness(1.0f);

	list<Module*>::iterator item;

	LOG("Application CleanUp--------------");
	for (item = moduleList.begin(); item != moduleList.end() && ret == true; ++item)
		ret = item._Ptr->_Myval->CleanUp();

	CleanImGui();

	moduleList.clear();

	return ret;
}

void Application::AddModule(Module* mod)
{
	moduleList.push_back(mod);
}

void Application::ProfilerLogic()
{
	if (lastSecFrameTime.Read() > 1000)
	{
		lastSecFrameTime.Start();
		prevLastSecFrameCount = lastSecFrameCount;
		lastSecFrameCount = 0;
	}

	float averageFps = float(frameCount) / startupTime.ReadSec();
	float secondsSinceStartup = startupTime.ReadSec();
	uint32 lastFrameMs = frameTime.Read();
	uint32 framesOnLastUpdate = prevLastSecFrameCount;

	// Frame Cap
	frameDelay = 1000 / fps;
	if (frameDelay > lastFrameMs)
	{
		SDL_Delay(frameDelay - lastFrameMs);
	}

	fpsLog.push_back(framesOnLastUpdate);
	fpsLog.push_back(0.0f);
	msLog.push_back(lastFrameMs);
	msLog.push_back(0.0f);

	while (fpsLog.size() > frameBarLimit * 2)
	{
		fpsLog.erase(fpsLog.begin());
		fpsLog.erase(fpsLog.begin());
	}

	while (msLog.size() > msBarLimit * 2)
	{
		msLog.erase(msLog.begin());
		msLog.erase(msLog.begin());
	}
}

bool Application::Load(string fName, FileContent fc)
{
	if (!loadRequest)
	{ // Reaching when the function is called anywhere in the code. Variable "loadFileName" is setted as the variable "file"
		loadRequest = true;
		fileName = fName;
		fileName += ".json";
		fileContent = fc;
	}
	else
	{ // Reaching at the end of the code iteration. Variable "file" is actually the variable "loadFileName"
		loadRequest = false;

		JSON_Value* file = json_parse_file(fName.c_str());;

		switch (fc)
		{
		case FileContent::PROJECT:
		{
			scene->SetWindowState(Windows::DEMO_W, json_object_get_boolean(json_object(file), "IsDemoWindowOpen"));
			scene->SetWindowState(Windows::CONFIG_W, json_object_get_boolean(json_object(file), "IsConfigWindowOpen"));
			scene->SetWindowState(Windows::OUTPUT_W, json_object_get_boolean(json_object(file), "IsOutputWindowOpen"));

			scene->SetProjectName(json_object_get_string(json_object(file), "ProjectName"));
			scene->SetTeamName(json_object_get_string(json_object(file), "TeamName"));

			fps = json_object_get_number(json_object(file), "FPS");
			frameBarLimit = json_object_get_number(json_object(file), "FrameBarLimit");
			msBarLimit = json_object_get_number(json_object(file), "MsBarLimit");

			scene->SetBrightness(json_object_get_number(json_object(file), "Brightness"));
			scene->SetWinDimensions({ json_object_get_number(json_object(file), "Window Width"), json_object_get_number(json_object(file), "Window Height") });
			scene->SetWinDimensionProportion(json_object_get_number(json_object(file), "Window Proportion Value"));

			scene->SetWindowSettings(WindowSettings::FULL_SCREEN, json_object_get_boolean(json_object(file), "Full Screen"));
			scene->SetWindowSettings(WindowSettings::FULL_DESKTOP, json_object_get_boolean(json_object(file), "Full Desktop"));
			scene->SetWindowSettings(WindowSettings::BORDERLESS, json_object_get_boolean(json_object(file), "Window Borderless"));
			scene->SetWindowSettings(WindowSettings::V_SYNC, json_object_get_boolean(json_object(file), "VSync"));
			scene->SetWindowSettings(WindowSettings::KEEP_PROPORTION, json_object_get_boolean(json_object(file), "Keep Proportion"));

			camera->SetSpeed(json_object_get_number(json_object(file), "Camera Speed"));
			camera->SetSensitivity(json_object_get_number(json_object(file), "Camera Sens"));

			scene->SetGeometryView(GeometryView::DEPTH_TEST, json_object_get_boolean(json_object(file), "Depth Test"));
			scene->SetGeometryView(GeometryView::CULL_FACE, json_object_get_boolean(json_object(file), "Cull Face"));
			scene->SetGeometryView(GeometryView::LIGHTING, json_object_get_boolean(json_object(file), "Lighting"));
			scene->SetGeometryView(GeometryView::COLOR_MATERIAL, json_object_get_boolean(json_object(file), "Color Material"));
			scene->SetGeometryView(GeometryView::TEXTURE_2D, json_object_get_boolean(json_object(file), "Texture 2D"));

			break;
		}
		case FileContent::CONFIG_PREFERENCES:
		{
			fps = json_object_get_number(json_object(file), "FPS");
			frameBarLimit = json_object_get_number(json_object(file), "FrameBarLimit");
			msBarLimit = json_object_get_number(json_object(file), "MsBarLimit");

			scene->SetBrightness(json_object_get_number(json_object(file), "Brightness"));
			scene->SetWinDimensions({ json_object_get_number(json_object(file), "Window Width"), json_object_get_number(json_object(file), "Window Height") });
			scene->SetWinDimensionProportion(json_object_get_number(json_object(file), "Window Proportion Value"));

			scene->SetWindowSettings(WindowSettings::FULL_SCREEN, json_object_get_boolean(json_object(file), "Full Screen"));
			scene->SetWindowSettings(WindowSettings::FULL_DESKTOP, json_object_get_boolean(json_object(file), "Full Desktop"));
			scene->SetWindowSettings(WindowSettings::BORDERLESS, json_object_get_boolean(json_object(file), "Window Borderless"));
			scene->SetWindowSettings(WindowSettings::V_SYNC, json_object_get_boolean(json_object(file), "VSync"));
			scene->SetWindowSettings(WindowSettings::KEEP_PROPORTION, json_object_get_boolean(json_object(file), "Keep Proportion"));

			camera->SetSpeed(json_object_get_number(json_object(file), "Camera Speed"));
			camera->SetSensitivity(json_object_get_number(json_object(file), "Camera Sens"));

			scene->SetGeometryView(GeometryView::DEPTH_TEST, json_object_get_boolean(json_object(file), "Depth Test"));
			scene->SetGeometryView(GeometryView::CULL_FACE, json_object_get_boolean(json_object(file), "Cull Face"));
			scene->SetGeometryView(GeometryView::LIGHTING, json_object_get_boolean(json_object(file), "Lighting"));
			scene->SetGeometryView(GeometryView::COLOR_MATERIAL, json_object_get_boolean(json_object(file), "Color Material"));
			scene->SetGeometryView(GeometryView::TEXTURE_2D, json_object_get_boolean(json_object(file), "Texture 2D"));

			break;
		}
		}

		json_value_free(file);
		fileName.clear();
	}

	return true;
}

bool Application::Save(string fName, FileContent fc)
{
	bool ret = true;
	if (!saveRequest)
	{ // Reaching when the function is called anywhere in the code. Variable "saveFileName" is setted as the variable "file"
		saveRequest = true;
		fileName = fName;
		fileName += ".json";
		fileContent = fc;
	}
	else
	{ // Reaching at the end of the code iteration. Variable "file" is actually the variable "saveFileName"
		saveRequest = false;

		JSON_Value* file = json_parse_file(fName.c_str());

		switch (fc)
		{
		case FileContent::PROJECT:
		{
			JSON_Value* schema = json_parse_string(
				"{"
				//Project
				//  Main
				"\nIsDemoWindowOpen: "
				"\nIsConfigWindowOpen: "
				"\nIsOutputWindowOpen: "
				"\nProject Name: "
				"\nTeam Name: "

				// Config Preferences
				//	 General  
				"\nFPS: "
				"\nFrameBarLimit: "
				"\nMsBarLimit: "
				"\nBrightness: "
				"\nWindow Width: "
				"\nWindow Height: "
				"\nFull Screen: "
				"\nFull Desktop: "
				"\nWindow Resizable: "
				"\nWindow Borderless: "
				"\nVSync: "
				"\nWindow Proportion Value: "
				"\nKeep Proportion: "
				"\nCamera Speed: "
				"\nCamera Sens: "

				//	 Geometry View
				"\nDepth Test: "
				"\nCull Face: "
				"\nLighting: "
				"\nColor Material: "
				"\nTexture 2D: "
				"\n}"
			);

			if (file == NULL || json_validate(schema, file) != JSONSuccess)
			{
				file = json_value_init_object();

				// General Engine - Project
				json_object_set_boolean(json_object(file), "IsDemoWindowOpen", this->scene->GetWindowState(Windows::DEMO_W));
				json_object_set_boolean(json_object(file), "IsConfigWindowOpen", this->scene->GetWindowState(Windows::CONFIG_W));
				json_object_set_boolean(json_object(file), "IsOutputWindowOpen", this->scene->GetWindowState(Windows::OUTPUT_W));

				// Project Information - Project
				json_object_set_string(json_object(file), "ProjectName", this->scene->GetProjectName().c_str());
				json_object_set_string(json_object(file), "TeamName", this->scene->GetTeamName().c_str());

				// Engine Performance - Preferences
				json_object_set_number(json_object(file), "FPS", fps);
				json_object_set_number(json_object(file), "FrameBarLimit", frameBarLimit);
				json_object_set_number(json_object(file), "MsBarLimit", msBarLimit);

				// Window Configuration - Preferences
					// General
				json_object_set_number(json_object(file), "Brightness", this->scene->GetBrightness());
				json_object_set_number(json_object(file), "Window Width", this->scene->GetWinDimensions().x);
				json_object_set_number(json_object(file), "Window Height", this->scene->GetWinDimensions().y);
				json_object_set_number(json_object(file), "Window Proportion Value", this->scene->GetWinDimensionProportion());

				json_object_set_boolean(json_object(file), "Full Screen", this->scene->GetWindowSettings(WindowSettings::FULL_SCREEN));
				json_object_set_boolean(json_object(file), "Full Desktop", this->scene->GetWindowSettings(WindowSettings::FULL_DESKTOP));
				json_object_set_boolean(json_object(file), "Window Resizable", this->scene->GetWindowSettings(WindowSettings::RESIZABLE));
				json_object_set_boolean(json_object(file), "Window Borderless", this->scene->GetWindowSettings(WindowSettings::BORDERLESS));
				json_object_set_boolean(json_object(file), "VSync", this->scene->GetWindowSettings(WindowSettings::V_SYNC));
				json_object_set_boolean(json_object(file), "Keep Proportion", this->scene->GetWindowSettings(WindowSettings::KEEP_PROPORTION));

				json_object_set_number(json_object(file), "Camera Speed", *this->camera->GetSpeed());
				json_object_set_number(json_object(file), "Camera Sens", *this->camera->GetSensitivity());

				// Geometry View - Preferences
				json_object_set_boolean(json_object(file), "Depth Test", this->scene->GetGeometryView(GeometryView::DEPTH_TEST));
				json_object_set_boolean(json_object(file), "Cull Face", this->scene->GetGeometryView(GeometryView::CULL_FACE));
				json_object_set_boolean(json_object(file), "Lighting", this->scene->GetGeometryView(GeometryView::LIGHTING));
				json_object_set_boolean(json_object(file), "Color Material", this->scene->GetGeometryView(GeometryView::COLOR_MATERIAL));
				json_object_set_boolean(json_object(file), "Texture 2D", this->scene->GetGeometryView(GeometryView::TEXTURE_2D));

				json_serialize_to_file(file, fName.c_str());

				json_value_free(schema);
			}
			break;
		}
		case FileContent::CONFIG_PREFERENCES:
		{
			JSON_Value* schema = json_parse_string(
				"{"
				// Config Preferences
				//	 General  
				"\nFPS: "
				"\nFrameBarLimit: "
				"\nMsBarLimit: "
				"\nBrightness: "
				"\nWindow Width: "
				"\nWindow Height: "
				"\nFull Screen: "
				"\nFull Desktop: "
				"\nWindow Resizable: "
				"\nWindow Borderless: "
				"\nVSync: "
				"\nWindow Proportion Value: "
				"\nKeep Proportion: "
				"\nCamera Speed: "
				"\nCamera Sens: "

				//	 Geometry View
				"\nDepth Test: "
				"\nCull Face: "
				"\nLighting: "
				"\nColor Material: "
				"\nTexture 2D: "
				"\n}"
			);

			if (file == NULL || json_validate(schema, file) != JSONSuccess)
			{
				file = json_value_init_object();

				json_object_set_number(json_object(file), "FPS", fps);
				json_object_set_number(json_object(file), "FrameBarLimit", frameBarLimit);
				json_object_set_number(json_object(file), "MsBarLimit", msBarLimit);

				json_object_set_number(json_object(file), "Brightness", this->scene->GetBrightness());
				json_object_set_number(json_object(file), "Window Width", this->scene->GetWinDimensions().x);
				json_object_set_number(json_object(file), "Window Height", this->scene->GetWinDimensions().y);
				json_object_set_number(json_object(file), "Window Proportion Value", this->scene->GetWinDimensionProportion());

				json_object_set_boolean(json_object(file), "Full Screen", this->scene->GetWindowSettings(WindowSettings::FULL_SCREEN));
				json_object_set_boolean(json_object(file), "Full Desktop", this->scene->GetWindowSettings(WindowSettings::FULL_DESKTOP));
				json_object_set_boolean(json_object(file), "Window Resizable", this->scene->GetWindowSettings(WindowSettings::RESIZABLE));
				json_object_set_boolean(json_object(file), "Window Borderless", this->scene->GetWindowSettings(WindowSettings::BORDERLESS));
				json_object_set_boolean(json_object(file), "VSync", this->scene->GetWindowSettings(WindowSettings::V_SYNC));
				json_object_set_boolean(json_object(file), "Keep Proportion", this->scene->GetWindowSettings(WindowSettings::KEEP_PROPORTION));

				json_object_set_number(json_object(file), "Camera Speed", *this->camera->GetSpeed());
				json_object_set_number(json_object(file), "Camera Sens", *this->camera->GetSensitivity());

				json_object_set_boolean(json_object(file), "Depth Test", this->scene->GetGeometryView(GeometryView::DEPTH_TEST));
				json_object_set_boolean(json_object(file), "Cull Face", this->scene->GetGeometryView(GeometryView::CULL_FACE));
				json_object_set_boolean(json_object(file), "Lighting", this->scene->GetGeometryView(GeometryView::LIGHTING));
				json_object_set_boolean(json_object(file), "Color Material", this->scene->GetGeometryView(GeometryView::COLOR_MATERIAL));
				json_object_set_boolean(json_object(file), "Texture 2D", this->scene->GetGeometryView(GeometryView::TEXTURE_2D));

				json_serialize_to_file(file, fName.c_str());

				json_value_free(schema);
			}
			break;
		}
		}

		json_value_free(file);
		fileName.clear();
	}
	return ret;
}

bool Application::SaveRestartPropierties()
{
	bool ret = true;

	JSON_Value* file = json_parse_file("NNE_Preferences_Default.json");

	JSON_Value* schema = json_parse_string(
		"{"
		"\nFPS: "
		"\nFrameBarLimit: "
		"\nMsBarLimit: "
		"\nBrightness: "
		"\nWindow Width: "
		"\nWindow Height: "
		"\nFull Screen: "
		"\nFull Desktop: "
		"\nWindow Resizable: "
		"\nWindow Borderless: "
		"\nVSync: "
		"\nWindow Proportion Value: "
		"\nKeep Proportion: "
		"\nCamera Speed: "
		"\nCamera Sens: "
		"\n}"
	);

	if (file == NULL || json_validate(schema, file) != JSONSuccess)
	{
		file = json_value_init_object();

		json_object_set_number(json_object(file), "FPS", 60);
		json_object_set_number(json_object(file), "FrameBarLimit", 80);
		json_object_set_number(json_object(file), "MsBarLimit", 80);

		json_object_set_number(json_object(file), "Brightness", 1);
		json_object_set_number(json_object(file), "Window Width", 1024);
		json_object_set_number(json_object(file), "Window Height", 768);
		json_object_set_number(json_object(file), "Window Proportion Value", 100);

		json_object_set_boolean(json_object(file), "Full Screen", false);
		json_object_set_boolean(json_object(file), "Full Desktop", false);
		json_object_set_boolean(json_object(file), "Window Resizable", this->scene->GetWindowSettings(WindowSettings::RESIZABLE));
		json_object_set_boolean(json_object(file), "Window Borderless", false);
		json_object_set_boolean(json_object(file), "VSync", true);
		json_object_set_boolean(json_object(file), "Keep Proportion", false);

		json_object_set_number(json_object(file), "Camera Speed", 0.5);
		json_object_set_number(json_object(file), "Camera Sens", 0.3);

		json_object_set_boolean(json_object(file), "Depth Test", false);
		json_object_set_boolean(json_object(file), "Cull Face", false);
		json_object_set_boolean(json_object(file), "Lighting", true);
		json_object_set_boolean(json_object(file), "Color Material", true);
		json_object_set_boolean(json_object(file), "Texture 2D", true);

		json_serialize_to_file(file, "NNE_Preferences_Default.json");

		json_value_free(schema);
	}

	json_value_free(file);
	fileName.clear();

	return ret;
}

bool Application::LoadRestartPropierties()
{
	bool ret = true;

	JSON_Value* file = json_parse_file("NNE_Preferences_Default.json");

	scene->SetWindowSettings(WindowSettings::RESIZABLE, json_object_get_boolean(json_object(file), "Window Resizable"));

	json_value_free(file);

	return ret;
}
