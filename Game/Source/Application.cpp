#include "Module.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleSceneIntro.h"
#include "ModuleCamera3D.h"
#include "ModuleRenderer3D.h"

#include "AudioSourceComponent.h"
#include "SpacialAudioSourceComponent.h"
#include "SwitchAudioSourceComponent.h"


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
	AddModule(input);
	
	// Scenes
	AddModule(scene);

	// Render
	AddModule(render);

	// Camera
	AddModule(camera);


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
		return bool(-1);
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

	fpsLog.push_back(float(framesOnLastUpdate));
	fpsLog.push_back(0.0f);
	msLog.push_back(float(lastFrameMs));
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

		JSON_Value* file = json_parse_file(fName.c_str());
		JSON_Value* cameraValue = json_object_get_value(json_object(file), "Camera");
		JSON_Array* cameraArray = json_value_get_array(cameraValue);
		JSON_Value* shapeValue = json_object_get_value(json_object(file), "Primitives");
		JSON_Array* shapeArray = json_value_get_array(shapeValue);

		switch (fc)
		{
		case FileContent::PROJECT:
		{
			scene->SetWindowState(Windows::DEMO_W, json_object_get_boolean(json_object(file), "IsDemoWindowOpen"));
			scene->SetWindowState(Windows::CONFIG_W, json_object_get_boolean(json_object(file), "IsConfigWindowOpen"));
			scene->SetWindowState(Windows::OUTPUT_W, json_object_get_boolean(json_object(file), "IsOutputWindowOpen"));
			scene->SetWindowState(Windows::HIERARCHY_W, json_object_get_boolean(json_object(file), "IsHierarchyWindowOpen"));

			Point3D lookPoint = {};

			ArrayRetrieveVector(cameraArray, &camera->X, 0);
			ArrayRetrieveVector(cameraArray, &camera->Y, 1);
			ArrayRetrieveVector(cameraArray, &camera->Z, 2);
			ArrayRetrieveVector(cameraArray, &camera->position, 3);
			ArrayRetrieveVector(cameraArray, &camera->reference, 4);
			ArrayRetrieveVector(cameraArray, &lookPoint, 5);
			camera->SetLookPoint(lookPoint);
			camera->CalculateViewMatrix();

			//----------------------------
			JsonLoadShapes(shapeArray, &scene->shapes);

			scene->SetProjectName(json_object_get_string(json_object(file), "ProjectName"));
			scene->SetTeamName(json_object_get_string(json_object(file), "TeamName"));

			fps = (int)json_object_get_number(json_object(file), "FPS");
			frameBarLimit = (int)json_object_get_number(json_object(file), "FrameBarLimit");
			msBarLimit = (int)json_object_get_number(json_object(file), "MsBarLimit");

			scene->SetBrightness(float(json_object_get_number(json_object(file), "Brightness")));
			scene->SetWinDimensions({ float(json_object_get_number(json_object(file), "Window Width")), float(json_object_get_number(json_object(file), "Window Height")) });
			scene->SetWinDimensionProportion(int(json_object_get_number(json_object(file), "Window Proportion Value")));

			scene->SetWindowSettings(WindowSettings::FULL_SCREEN, json_object_get_boolean(json_object(file), "Full Screen"));
			scene->SetWindowSettings(WindowSettings::FULL_DESKTOP, json_object_get_boolean(json_object(file), "Full Desktop"));
			scene->SetWindowSettings(WindowSettings::BORDERLESS, json_object_get_boolean(json_object(file), "Window Borderless"));
			scene->SetWindowSettings(WindowSettings::V_SYNC, json_object_get_boolean(json_object(file), "VSync"));
			scene->SetWindowSettings(WindowSettings::KEEP_PROPORTION, json_object_get_boolean(json_object(file), "Keep Proportion"));

			camera->SetSpeed(float(json_object_get_number(json_object(file), "Camera Speed")));
			camera->SetSensitivity(float(json_object_get_number(json_object(file), "Camera Sens")));

			scene->SetGeometryView(GeometryView::DEPTH_TEST, json_object_get_boolean(json_object(file), "Depth Test"));
			scene->SetGeometryView(GeometryView::CULL_FACE, json_object_get_boolean(json_object(file), "Cull Face"));
			scene->SetGeometryView(GeometryView::LIGHTING, json_object_get_boolean(json_object(file), "Lighting"));
			scene->SetGeometryView(GeometryView::COLOR_MATERIAL, json_object_get_boolean(json_object(file), "Color Material"));
			scene->SetGeometryView(GeometryView::TEXTURE_2D, json_object_get_boolean(json_object(file), "Texture 2D"));

			break;
		}
		case FileContent::CONFIG_PREFERENCES:
		{
			fps = int(json_object_get_number(json_object(file), "FPS"));
			frameBarLimit = int(json_object_get_number(json_object(file), "FrameBarLimit"));
			msBarLimit = int(json_object_get_number(json_object(file), "MsBarLimit"));

			scene->SetBrightness(float(json_object_get_number(json_object(file), "Brightness")));
			scene->SetWinDimensions({ float(json_object_get_number(json_object(file), "Window Width")), float(json_object_get_number(json_object(file), "Window Height")) });
			scene->SetWinDimensionProportion(int(json_object_get_number(json_object(file), "Window Proportion Value")));

			scene->SetWindowSettings(WindowSettings::FULL_SCREEN, json_object_get_boolean(json_object(file), "Full Screen"));
			scene->SetWindowSettings(WindowSettings::FULL_DESKTOP, json_object_get_boolean(json_object(file), "Full Desktop"));
			scene->SetWindowSettings(WindowSettings::BORDERLESS, json_object_get_boolean(json_object(file), "Window Borderless"));
			scene->SetWindowSettings(WindowSettings::V_SYNC, json_object_get_boolean(json_object(file), "VSync"));
			scene->SetWindowSettings(WindowSettings::KEEP_PROPORTION, json_object_get_boolean(json_object(file), "Keep Proportion"));

			camera->SetSpeed(float(json_object_get_number(json_object(file), "Camera Speed")));
			camera->SetSensitivity(float(json_object_get_number(json_object(file), "Camera Sens")));

			scene->SetGeometryView(GeometryView::DEPTH_TEST, json_object_get_boolean(json_object(file), "Depth Test"));
			scene->SetGeometryView(GeometryView::CULL_FACE, json_object_get_boolean(json_object(file), "Cull Face"));
			scene->SetGeometryView(GeometryView::LIGHTING, json_object_get_boolean(json_object(file), "Lighting"));
			scene->SetGeometryView(GeometryView::COLOR_MATERIAL, json_object_get_boolean(json_object(file), "Color Material"));
			scene->SetGeometryView(GeometryView::TEXTURE_2D, json_object_get_boolean(json_object(file), "Texture 2D"));

			break;
		}
		}

		//json_value_free(cameraValue);
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
		JSON_Value* cameraValue = json_value_init_array();
		JSON_Array* cameraArray = json_value_get_array(cameraValue);
		JSON_Value* shapeValue = json_value_init_array();
		JSON_Array* shapeArray = json_value_get_array(shapeValue);

		switch (fc)
		{
		case FileContent::PROJECT:
		{
			JSON_Value* schema = json_parse_string(
				"{"
				//Project
				//  Main
				"IsDemoWindowOpen: "
				"IsConfigWindowOpen: "
				"IsOutputWindowOpen: "
				"IsHierarchyWindowOpen: "
				"Project Name: "
				"Team Name: "

				//  Camera   
				"Camera:"

				// Shapes
				"Primitives:"

				// Config Preferences
				//	 General  
				"FPS: "
				"FrameBarLimit: "
				"MsBarLimit: "
				"Brightness: "
				"Window Width: "
				"Window Height: "
				"Full Screen: "
				"Full Desktop: "
				"Window Resizable: "
				"Window Borderless: "
				"VSync: "
				"Window Proportion Value: "
				"Keep Proportion: "
				"Camera Speed: "
				"Camera Sens: "

				//	 Geometry View
				"Depth Test: "
				"Cull Face: "
				"Lighting: "
				"Color Material: "
				"Texture 2D: "
			);

			//Test

			if (file == NULL || json_validate(schema, file) != JSONSuccess)
			{
				file = json_value_init_object();

				// General Engine - Project
				json_object_set_boolean(json_object(file), "IsDemoWindowOpen", this->scene->GetWindowState(Windows::DEMO_W));
				json_object_set_boolean(json_object(file), "IsConfigWindowOpen", this->scene->GetWindowState(Windows::CONFIG_W));
				json_object_set_boolean(json_object(file), "IsOutputWindowOpen", this->scene->GetWindowState(Windows::OUTPUT_W));
				json_object_set_boolean(json_object(file), "IsHierarchyWindowOpen", this->scene->GetWindowState(Windows::HIERARCHY_W));

				// General Engine - Camera
				ArrayAppendVector(cameraArray, camera->X);
				ArrayAppendVector(cameraArray, camera->Y);
				ArrayAppendVector(cameraArray, camera->Z);
				ArrayAppendVector(cameraArray, camera->position);
				ArrayAppendVector(cameraArray, camera->reference);
				ArrayAppendVector(cameraArray, camera->GetLookPoint());
				json_object_set_value(json_object(file), "Camera", cameraValue);

				// General Engine - Primitives
				JsonSaveShapes(shapeArray, &scene->shapes, 1);
				json_object_set_value(json_object(file), "Primitives", shapeValue);

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
				"FPS: "
				"FrameBarLimit: "
				"MsBarLimit: "
				"Brightness: "
				"Window Width: "
				"Window Height: "
				"Full Screen: "
				"Full Desktop: "
				"Window Resizable: "
				"Window Borderless: "
				"VSync: "
				"Window Proportion Value: "
				"Keep Proportion: "
				"Camera Speed: "
				"Camera Sens: "

				//	 Geometry View
				"Depth Test: "
				"Cull Face: "
				"Lighting: "
				"Color Material: "
				"Texture 2D: "
				"}"
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

		//json_value_free(cameraValue);
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

		json_object_set_boolean(json_object(file), "Depth Test", true);
		json_object_set_boolean(json_object(file), "Cull Face", false);
		json_object_set_boolean(json_object(file), "Lighting", false);
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

void Application::JsonSaveShapes(JSON_Array* arr, vector<Shape3D*>* shapes, int offset)
{
	for (unsigned int i = offset; i < shapes->size(); i++)
	{
		Shape3D* shape = shapes->at(i);
		JSON_Value* shapeValue = json_value_init_array();
		JSON_Array* shapeArray = json_value_get_array(shapeValue);

		json_array_append_boolean(shapeArray, shape->axis); // Axis (1) - 0
		json_array_append_boolean(shapeArray, shape->draw); // Draw (1) - 1
		json_array_append_boolean(shapeArray, shape->solid); // Solid (1) - 2
		json_array_append_boolean(shapeArray, shape->edges); // Edges (1) - 3
		json_array_append_boolean(shapeArray, shape->normals); // Normals (1) - 4
		json_array_append_boolean(shapeArray, shape->hasParent); // Has Parent (1) - 5

		ArrayAppendVector(shapeArray, shape->GetPosition()); // Position (3) - 6
		json_array_append_number(shapeArray, shape->GetRotation().angle); // Angle (1) - 9
		ArrayAppendVector(shapeArray, shape->GetRotation().GetPlane()); // Rotation (3) - 10
		ArrayAppendVector(shapeArray, shape->GetScale()); // Scale (3) - 13

		json_array_append_string(shapeArray, shape->WriteShapeType().c_str()); // Type (1) - 16
		json_array_append_string(shapeArray, shape->GetName()); // Name (1) - 17
		json_array_append_number(shapeArray, shape->id); // Id (1) - 18

		switch (shape->GetShapeType())
		{
		case ShapeType::LINE3D:
		{
			Line3D* l = (Line3D*)shape;
			ArrayAppendVector(shapeArray, l->GetSecondVertex()); // Second Vertex (3) - 19
			break;
		}
		case ShapeType::PYRAMID3D:
		{
			Pyramid3D* py = (Pyramid3D*)shape;
			json_array_append_number(shapeArray, py->GetHeight()); // Height (1) - 19
			break;
		}
		case ShapeType::CYLINDER3D:
		{
			Cylinder3D* cy = (Cylinder3D*)shape;
			json_array_append_number(shapeArray, cy->GetHeight()); // Height (1) - 19
			json_array_append_number(shapeArray, cy->GetRadius()); // Radius (1) - 20
			json_array_append_number(shapeArray, cy->GetSegments()); // Segments (1) - 21
			break;
		}
		case ShapeType::PLANE3D:
		{
			Plane3D* p = (Plane3D*)shape;
			ArrayAppendVector(shapeArray, p->GetNormal()); // Normal (3) - 19
			break;
		}
		case ShapeType::SPHERE3D:
		{
			Sphere3D* s = (Sphere3D*)shape;
			json_array_append_number(shapeArray, s->GetRadius()); // Radius (1) - 19
			json_array_append_number(shapeArray, s->GetSubdivision()); // Subdivisions (1) - 20
			break;
		}
		case ShapeType::MODEL3D:
		{
			Model* m = (Model*)shape;
			json_array_append_string(shapeArray, m->filePath.c_str()); // Path File (1) - 19
			json_array_append_string(shapeArray, m->texturePath.c_str()); // Path File (1) - 20

			break;
		}
		}

		JSON_Value* childValue = json_value_init_array();
		JSON_Array* childArray = json_value_get_array(childValue);
		if (!shape->childs.empty()) JsonSaveShapes(childArray, &shape->childs); // Childs (1) - Depends
		json_array_append_value(shapeArray, childValue);

		JSON_Value* componentValue = json_value_init_array();
		JSON_Array* componentArray = json_value_get_array(componentValue);
		if (!shape->components.empty()) JsonSaveComponents(componentArray, &shape->components); // Childs (1) - Depends
		json_array_append_value(shapeArray, componentValue);

		json_array_append_value(arr, shapeValue);
	}
}

void Application::JsonLoadShapes(JSON_Array* arr, vector<Shape3D*>* shapes, Shape3D* parent)
{
	for (unsigned int i = 0; i < json_array_get_count(arr); i++)
	{
		JSON_Value* shapeValue = json_array_get_value(arr, i);
		JSON_Array* shapeArray = json_value_get_array(shapeValue);

		Empty3D c({0, 0, 0});
		Point3D position = {}, planeNormal = {}, scale = {};

		bool axis = json_array_get_boolean(shapeArray, 0);
		bool draw = json_array_get_boolean(shapeArray, 1);
		bool solid = json_array_get_boolean(shapeArray, 2);
		bool edges = json_array_get_boolean(shapeArray, 3);
		bool normals = json_array_get_boolean(shapeArray, 4);
		bool hasParent = json_array_get_boolean(shapeArray, 5);

		ArrayRetrieveVector(shapeArray, &position, 6, false);
		float angle = (float)json_array_get_number(shapeArray, 9);
		ArrayRetrieveVector(shapeArray, &planeNormal, 10, false);
		ArrayRetrieveVector(shapeArray, &scale, 13, false);

		const char* typeChar = json_array_get_string(shapeArray, 16);
		string type = typeChar;
		const char* nameChar = json_array_get_string(shapeArray, 17);
		string name = nameChar;
		int id = (int)json_array_get_number(shapeArray, 18);

		int lastSpace = 18;

		switch (c.ReadShapeType(type.c_str()))
		{
		case ShapeType::CUBE3D:
		{
			shapes->push_back(new Cube3D(position, scale, Rotation{angle, planeNormal.x, planeNormal.y, planeNormal.z}));
			break;
		}
		case ShapeType::LINE3D:
		{
			lastSpace = 21;
			Point3D secondVertex = {};
			ArrayRetrieveVector(shapeArray, &secondVertex, 19, false); // Second Vertex (3)
			shapes->push_back(new Line3D(position, secondVertex, scale.x));
			break;
		}
		case ShapeType::PYRAMID3D:
		{
			lastSpace = 19;
			float height = (float)json_array_get_number(shapeArray, 19);
			shapes->push_back(new Pyramid3D(position, height, scale, Rotation{ angle, planeNormal.x, planeNormal.y, planeNormal.z }));
			break;
		}
		case ShapeType::CYLINDER3D:
		{
			lastSpace = 21;
			float height = (float)json_array_get_number(shapeArray, 19);
			float radius = (float)json_array_get_number(shapeArray, 20);
			int segments = (int)json_array_get_number(shapeArray, 21);
			shapes->push_back(new Cylinder3D(position, segments, height, radius, scale, Rotation{ angle, planeNormal.x, planeNormal.y, planeNormal.z }));
			break;
		}
		case ShapeType::PLANE3D:
		{
			lastSpace = 21;
			Point3D normal = {};
			ArrayRetrieveVector(shapeArray, &normal, 19, false); // Normal (3)
			shapes->push_back(new Plane3D(position, normal, scale, Rotation{ angle, planeNormal.x, planeNormal.y, planeNormal.z }));
			break;
		}
		case ShapeType::SPHERE3D:
		{
			lastSpace = 20;
			float radius = (float)json_array_get_number(shapeArray, 19);
			int subdivisions = (int)json_array_get_number(shapeArray, 20);
			shapes->push_back(new Sphere3D(position, scale, radius, subdivisions, Rotation{ angle, planeNormal.x, planeNormal.y, planeNormal.z }));
			break;
		}
		case ShapeType::MODEL3D:
		{
			lastSpace = 20;
			string filePath = json_array_get_string(shapeArray, 19);
			string texturePath = json_array_get_string(shapeArray, 20);
			Model* m = new Model(position, scale, Rotation{ angle, planeNormal.x, planeNormal.y, planeNormal.z });
			m->LoadModel(filePath.c_str(), texturePath.c_str());
			shapes->push_back(m);
			break;
		}
		case ShapeType::EMPTY3D:
		{
			shapes->push_back(new Empty3D(position, scale, Rotation{ angle, planeNormal.x, planeNormal.y, planeNormal.z }));
			break;
		}
		}

		Shape3D* s = shapes->at(shapes->size() - 1);
		s->axis = axis;
		s->draw = draw;
		s->solid = solid;
		s->edges = edges;
		s->normals = normals;
		s->hasParent = hasParent;
		s->parent = parent;
		s->SetName(name.c_str());
		s->id = id;

		if (JSON_Value* childValue = json_array_get_value(shapeArray, lastSpace + 1))
		{
			JSON_Array* childArray = json_value_get_array(childValue);
			JsonLoadShapes(childArray, &s->childs, s);
		}

		if (JSON_Value* compValue = json_array_get_value(shapeArray, lastSpace + 2))
		{
			JSON_Array* compArray = json_value_get_array(compValue);
			JsonLoadComponents(compArray, &s->components);
		}
	}
}

void Application::JsonSaveComponents(JSON_Array* arr, vector<Component*>* comps)
{
	for (unsigned int i = 0; i < comps->size(); i++)
	{
		Component* comp = comps->at(i);
		JSON_Value* compValue = json_value_init_array();
		JSON_Array* compArray = json_value_get_array(compValue);

		bool openFactor[2] = {};
		openFactor[0] = comp->GetEditorOpenState(&openFactor[1]);
		json_array_append_boolean(compArray, openFactor[0]); // Open (1) - 0
		json_array_append_boolean(compArray, openFactor[1]); // PrevOpen (1) - 1
		json_array_append_number(compArray, (int)comp->GetComponentID()); // ID (1) - 2

		switch (comp->GetComponentID())
		{
			case AUDIO_SOURCE_COMPONENT:
			{
				AudioSourceComponent* a = (AudioSourceComponent*)comp;

				json_array_append_boolean(compArray, a->GetMute()); // Mute (1) - 3
				json_array_append_boolean(compArray, a->GetPlayOnStart()); // Play On Start (1) - 4
				json_array_append_boolean(compArray, a->GetLoop()); // Loop (1) - 5
				json_array_append_boolean(compArray, a->GetBypass()); // Bypass (1) - 6

				json_array_append_number(compArray, a->GetVolume()); // Volume (1) - 7
				json_array_append_number(compArray, a->GetPan()); // Pan (1) - 8
				json_array_append_number(compArray, a->GetTranspose()); // Transpose (1) - 9

				const char* path = a->IsTrackLoaded() ? a->GetPath() : "no track";
				json_array_append_string(compArray, path); // Path (1) - 10

				break;
			}
			case SPACIAL_AUDIO_SOURCE_COMPONENT:
			{
				SpacialAudioSourceComponent* a = (SpacialAudioSourceComponent*)comp;

				json_array_append_boolean(compArray, a->GetMute()); // Mute (1) - 3
				json_array_append_boolean(compArray, a->GetPlayOnStart()); // Play On Start (1) - 4
				json_array_append_boolean(compArray, a->GetLoop()); // Loop (1) - 5
				json_array_append_boolean(compArray, a->GetBypass()); // Bypass (1) - 6
				json_array_append_boolean(compArray, a->GetDoppler()); // Doppler (1) - 7
				json_array_append_boolean(compArray, a->GetDopplerWindow()); //  Doppler Window (1) - 8
				json_array_append_boolean(compArray, a->GetDopplerEffect()); // Doppler Effect (1) - 9

				json_array_append_number(compArray, a->GetVolume()); // Volume (1) - 10
				json_array_append_number(compArray, a->GetTranspose()); // Transpose (1) - 11
				json_array_append_number(compArray, a->GetVelocity()); // Velocity (1) - 12
				json_array_append_number(compArray, a->GetFactor()); // Factor (1) - 13

				const char* path = a->IsTrackLoaded() ? a->GetPath() : "no track";
				json_array_append_string(compArray, path); // Path (1) - 14

				break;
			}
			case SWITCH_AUDIO_SOURCE_COMPONENT:
			{
				SwitchAudioSourceComponent* a = (SwitchAudioSourceComponent*)comp;
				json_array_append_number(compArray, a->GetTotalTracks()); // Total Tracks (1) - 3
				json_array_append_number(compArray, a->GetCurrentTrackEditor()); // Current Track Editor (1) - 4
				json_array_append_number(compArray, a->GetNextSwitchTrack()); // Next Switch Track (1) - 5
				json_array_append_number(compArray, a->GetFadeTime()); // Fade Time (1) - 6

				for (unsigned int i = 0; i < a->GetTrackInstances()->size(); i++)
				{
					TrackInstance* t = &a->GetTrackInstances()->at(i);
					JSON_Value* trackValue = json_value_init_array();
					JSON_Array* trackArray = json_value_get_array(trackValue);

					json_array_append_boolean(trackArray, t->GetMute()); // Mute (1) - 0
					json_array_append_boolean(trackArray, t->GetPlayOnStart()); // Play On Start (1) - 1
					json_array_append_boolean(trackArray, t->GetLoop()); // Loop (1) - 2
					json_array_append_boolean(trackArray, t->GetBypass()); // Bypass (1) - 3

					json_array_append_number(trackArray, t->GetVolume()); // Volume (1) - 4
					json_array_append_number(trackArray, t->GetPan()); // Pan (1) - 5
					json_array_append_number(trackArray, t->GetTranspose()); // Transpose (1) - 6

					const char* path = t->IsTrackLoaded() ? t->GetPath() : "no track";
					json_array_append_string(trackArray, path); // Path (1) - 7

					json_array_append_value(compArray, trackValue);
				}

				break;
			}
		}

		json_array_append_value(arr, compValue);
	}
}

void Application::JsonLoadComponents(JSON_Array* arr, vector<Component*>* comps)
{
	for (unsigned int i = 0; i < json_array_get_count(arr); i++)
	{
		JSON_Value* compValue = json_array_get_value(arr, i);
		JSON_Array* compArray = json_value_get_array(compValue);

		bool open = json_array_get_boolean(compArray, 0);
		bool prevOpen = json_array_get_boolean(compArray, 1);
		int id = json_array_get_number(compArray, 2);

		switch (id)
		{
			case 0: // AUDIO SOURCE COMPONENT
			{
				bool mute = json_array_get_boolean(compArray, 3);
				bool playOnStart = json_array_get_boolean(compArray, 4);
				bool loop = json_array_get_boolean(compArray, 5);
				bool bypass = json_array_get_boolean(compArray, 6);

				float volume = json_array_get_number(compArray, 7);
				float pan = json_array_get_number(compArray, 8);
				float transpose = json_array_get_number(compArray, 9);

				AudioSourceComponent* ASC = new AudioSourceComponent(open, prevOpen, volume, pan, transpose, mute, playOnStart, loop, bypass, &this->scene->gameTimer, this->scene->GetAudioSystem());
				const char* path = json_array_get_string(compArray, 10);
				if (!SameString(path, "no track")) ASC->LoadTrackFromLoadingProject(path);

				comps->push_back(ASC);
				break;
			}
			case 1: // SWITCH AUDIO SOURCE COMPONENT
			{
				int totalTracks = json_array_get_number(compArray, 3);
				int currentTrackEditor = json_array_get_number(compArray, 4);
				int nextSwitchTrack = json_array_get_number(compArray, 5);
				float fadeTime = json_array_get_number(compArray, 6);

				vector<TrackInstance> tracks;

				for (unsigned int i = 0; i < totalTracks; i++)
				{
					JSON_Value* trackValue = json_array_get_value(compArray, 7 + i);
					JSON_Array* trackArray = json_value_get_array(trackValue);

					bool mute = json_array_get_boolean(trackArray, 0);
					bool playOnStart = json_array_get_boolean(trackArray, 1);
					bool loop = json_array_get_boolean(trackArray, 2);
					bool bypass = json_array_get_boolean(trackArray, 3);

					float volume = json_array_get_number(trackArray, 4);
					float pan = json_array_get_number(trackArray, 5);
					float transpose = json_array_get_number(trackArray, 6);

					const char* path = json_array_get_string(trackArray, 7);
					if (SameString(path, "no track")) path = nullptr;

					tracks.push_back(TrackInstance(volume, pan, transpose, mute, playOnStart, loop,bypass, path, this->scene->GetAudioSystem()));
				}

				SwitchAudioSourceComponent* SwASC = new SwitchAudioSourceComponent(open, prevOpen, totalTracks, currentTrackEditor, nextSwitchTrack, fadeTime, tracks, &this->scene->gameTimer, this->scene->GetAudioSystem(), this->input);
				comps->push_back(SwASC);

				break;
			}
			case 2: // SPACIAL AUDIO SOURCE COMPONENT
			{
				bool mute = json_array_get_boolean(compArray, 3);
				bool playOnStart = json_array_get_boolean(compArray, 4);
				bool loop = json_array_get_boolean(compArray, 5);
				bool bypass = json_array_get_boolean(compArray, 6);
				bool doppler = json_array_get_boolean(compArray, 7);
				bool dopplerWindow = json_array_get_boolean(compArray, 8);
				bool dopplerEffect = json_array_get_boolean(compArray, 9);

				float volume = json_array_get_number(compArray, 10);
				float transpose = json_array_get_number(compArray, 11);
				float velocity = json_array_get_number(compArray, 12);
				float factor = json_array_get_number(compArray, 13);

				SpacialAudioSourceComponent* SASC = new SpacialAudioSourceComponent(open, prevOpen, volume, transpose, velocity, factor, doppler, dopplerWindow, dopplerEffect, mute, playOnStart, loop, bypass, &this->scene->gameTimer, this->scene->GetAudioSystem(), &this->camera->position, &this->camera->speed);
				const char* path = json_array_get_string(compArray, 14);
				if (!SameString(path, "no track")) SASC->LoadTrackFromLoadingProject(path);

				comps->push_back(SASC);

				break;
			}
		}
	}
}
