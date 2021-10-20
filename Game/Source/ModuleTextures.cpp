#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTextures.h"

ModuleTextures::ModuleTextures(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleTextures::~ModuleTextures()
{}

bool ModuleTextures::Init()
{
	bool ret = true;

	return ret;
}

bool ModuleTextures::CleanUp()
{
	return true;
}
