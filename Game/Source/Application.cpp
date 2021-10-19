#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"

#include "Primitive.h"

#include "../Source/External/ImGui/imgui.h"
#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl2.h"

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

	renderer = new ModuleRender(this);
	window = new ModuleWindow(this);
	textures = new ModuleTextures(this);
	input = new ModuleInput(this);
	scene = new ModuleScene(this);

	// Main Modules
	AddModule(window);
	AddModule(renderer);
	AddModule(textures);
	AddModule(input);
	
	// Scenes
	AddModule(scene);

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

	list<Module*>::iterator item;

	LOG("Application Init --------------");
	for (item = moduleList.begin(); item != moduleList.end(); ++item) 
		ret = item._Ptr->_Myval->Init();

	if (InitImGui() == false)
	{
		LOG("InitImGui exits with ERROR");
		ret = false;
	}

	LOG("Application Start --------------");
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


	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags wFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
	mainWindow = SDL_CreateWindow("Dear ImGui SDL2+OpenGL3 example", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, wFlags);
	SDL_GLContext gl_context = SDL_GL_CreateContext(mainWindow);
	SDL_GL_MakeCurrent(mainWindow, gl_context);
	SDL_GL_SetSwapInterval(1); // Enable vsync

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	//io.ConfigViewportsNoTaskBarIcon = true;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	// Setup Platform/Renderer backends
	ImGui_ImplSDL2_InitForOpenGL(mainWindow, gl_context);
	ImGui_ImplOpenGL2_Init();

	// Load Fonts
	// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
	// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
	// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
	// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
	// - Read 'docs/FONTS.md' for more instructions and details.
	// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
	//io.Fonts->AddFontDefault();
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
	//io.Fonts->AddFontFromFileTTF("../../misc/fonts/ProggyTiny.ttf", 10.0f);
	//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
	//IM_ASSERT(font != NULL);

	PERF_PEEK(ptimer);

	return ret;
}

void Application::CleanImGui()
{
	ImGui_ImplOpenGL2_Shutdown();
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
			this->scene->SetWindowState(Windows::DEMO_W, json_object_get_boolean(json_object(file), "IsDemoWindowOpen"));
			this->scene->SetWindowState(Windows::CONFIG_W, json_object_get_boolean(json_object(file), "IsConfigWindowOpen"));
			this->scene->SetWindowState(Windows::OUTPUT_W, json_object_get_boolean(json_object(file), "IsOutputWindowOpen"));

			this->scene->SetProjectName(json_object_get_string(json_object(file), "ProjectName"));
			this->scene->SetTeamName(json_object_get_string(json_object(file), "TeamName"));

			break;
		}
		case FileContent::CONFIG_PREFERENCES:
		{
			frameBarLimit = json_object_get_number(json_object(file), "FrameBarLimit");
			msBarLimit = json_object_get_number(json_object(file), "MsBarLimit");

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
				"\nIsDemoWindowOpen: "
				"\nIsConfigWindowOpen: "
				"\nIsOutputWindowOpen: "
				"\nProject Name: "
				"\nTeam Name: "
				"\n}"
			);

			if (file == NULL || json_validate(schema, file) != JSONSuccess)
			{
				file = json_value_init_object();

				json_object_set_boolean(json_object(file), "IsDemoWindowOpen", this->scene->GetWindowState(Windows::DEMO_W));
				json_object_set_boolean(json_object(file), "IsConfigWindowOpen", this->scene->GetWindowState(Windows::CONFIG_W));
				json_object_set_boolean(json_object(file), "IsOutputWindowOpen", this->scene->GetWindowState(Windows::OUTPUT_W));

				json_object_set_string(json_object(file), "ProjectName", this->scene->GetProjectName().c_str());
				json_object_set_string(json_object(file), "TeamName", this->scene->GetTeamName().c_str());

				json_serialize_to_file(file, fName.c_str());

				json_value_free(schema);
			}
			break;
		}
		case FileContent::CONFIG_PREFERENCES:
		{
			JSON_Value* schema = json_parse_string(
				"{"
				"\nFrameBarLimit: "
				"\nMsBarLimit: "
				"\n}"
			);

			if (file == NULL || json_validate(schema, file) != JSONSuccess)
			{
				file = json_value_init_object();

				json_object_set_number(json_object(file), "FrameBarLimit", frameBarLimit);
				json_object_set_number(json_object(file), "MsBarLimit", msBarLimit);

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