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
	LOG("Init Image library");
	bool ret = true;

	return ret;
}

bool ModuleTextures::CleanUp()
{
	//list<Textura*>::iterator item;

	//LOG("Application CleanUp --------------");
	//for (item = moduleList.begin(); item != moduleList.end(); ++item)
	//	SDL_DestroyTexture(item->data);

	//textures.clear();
	return true;
}
