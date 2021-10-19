#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"

ModuleScene::ModuleScene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleScene::~ModuleScene()
{}

bool ModuleScene::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	app->renderer->camera.x = app->renderer->camera.y = 0;

	// Create Scenes
	editor = new EditorScene(app);

	// Set first state of the Engine
	scene = Scenes::EDITOR;

	return ret;
}

update_status ModuleScene::Update()
{
	bool ret = true;

	switch (scene)
	{
	case Scenes::EDITOR: ret = UpdateEditor(); break;
	}

	if (sceneChangeRequest) ret = SetScene(changeScene);

	if (!ret) return UPDATE_STOP;
	return UPDATE_CONTINUE;
}

bool ModuleScene::UpdateEditor()
{
	bool ret = true;

	ret = editor->Update();

	return ret;
}

bool ModuleScene::SetScene(Scenes newScene)
{
	bool ret = true;

	if (newScene == scene) return true;

	if (!sceneChangeRequest)
	{
		sceneChangeRequest = true;
		changeScene = newScene;
	}
	else
	{
		CleanUpScene(scene);

		prevScene = scene;
		scene = newScene;

		switch (newScene)
		{
		case Scenes::EDITOR: ret = editor->Start();; break;
		}

		sceneChangeRequest = false;
		changeScene = Scenes::NO_SCENE;
	}

	return ret;
}

bool ModuleScene::CleanUpScene(Scenes scene)
{
	bool ret = true;
	switch (scene)
	{
	case Scenes::EDITOR: ret = editor->CleanUp(); break;
	}

	return ret;
}

bool ModuleScene::CleanUp()
{
	LOG("Unloading Scene");
	bool ret = true;

	ret = CleanUpScene(scene);

	return ret;
}
