#ifndef __ModuleWindow_H__
#define __ModuleWindow_H__

#include "Module.h"
#include "External/SDL/include/SDL.h"

class Application;
struct SDL_Window;

class ModuleWindow : public Module
{
public:

	ModuleWindow(Application* app, bool start_enabled = true);

	// Destructor
	virtual ~ModuleWindow();

	bool Init();
	bool CleanUp();

	void SetWinFullScreen(bool full);
	void SetWinDFullScreen(bool full);
	void SetWinResizable(bool resizable);
	void SetWinBorders(bool border);

	SDL_Window* mainWindow = nullptr;

};

#endif // __ModuleWindow_H__