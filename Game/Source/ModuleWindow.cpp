#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "External/SDL/include/SDL.h"
#include "ModuleSceneIntro.h"

#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl2.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	mainWindow = NULL;
}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Init()
{
	bool ret = true;

	app->Load("NNE_Preferences_Default", FileContent::CONFIG_PREFERENCES);
	app->LoadRestartPropierties();

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags wFlags;
	if (!app->scene->GetWindowSettings(WindowSettings::RESIZABLE)) wFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	else wFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

	mainWindow = SDL_CreateWindow("No Name Engine - Project Editor", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720, wFlags);
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

	return ret;
}

//bool ModuleWindow::Init()
//{
//	LOG("Init SDL window & surface");
//	bool ret = true;
//
//	if (SDL_Init(SDL_INIT_VIDEO) < 0)
//	{
//		LOG("SDL_VIDEO could not initialize! SDL_Error: %s\n", SDL_GetError());
//		ret = false;
//	}
//	else
//	{
//		//Create window
//		int width = SCREEN_WIDTH * SCREEN_SIZE;
//		int height = SCREEN_HEIGHT * SCREEN_SIZE;
//		Uint32 flags = SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN;
//
//		//Use OpenGL 2.1
//		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
//		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
//
//		if (WIN_FULLSCREEN == true)
//		{
//			flags |= SDL_WINDOW_FULLSCREEN;
//		}
//
//		if (WIN_RESIZABLE == true)
//		{
//			flags |= SDL_WINDOW_RESIZABLE;
//		}
//
//		if (WIN_BORDERLESS == true)
//		{
//			flags |= SDL_WINDOW_BORDERLESS;
//		}
//
//		if (WIN_FULLSCREEN_DESKTOP == true)
//		{
//			flags |= SDL_WINDOW_FULLSCREEN_DESKTOP;
//		}
//
//		mainWindow = SDL_CreateWindow("Hey", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, flags);
//
//		if (mainWindow == NULL)
//		{
//			LOG("Window could not be created! SDL_Error: %s\n", SDL_GetError());
//			ret = false;
//		}
//		else
//		{
//			//Get window surface
//			screen_surface = SDL_GetWindowSurface(mainWindow);
//		}
//	}
//
//	return ret;
//}

bool ModuleWindow::CleanUp()
{
	LOG("Destroying SDL window and quitting all SDL systems");

	return true;
}

void ModuleWindow::SetWinFullScreen(bool full)
{
	if (full) SDL_SetWindowFullscreen(app->window->mainWindow, SDL_WINDOW_FULLSCREEN);
	else if (!full) SDL_SetWindowFullscreen(app->window->mainWindow, 0);
}

void ModuleWindow::SetWinDFullScreen(bool full)
{
	if (full) SDL_SetWindowFullscreen(app->window->mainWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
	else if (!full) SDL_SetWindowFullscreen(app->window->mainWindow, 0);
}

void ModuleWindow::SetWinBorders(bool border)
{
	SDL_SetWindowBordered(mainWindow, (SDL_bool)!border);
}

void ModuleWindow::SetWinBrightness(float bright)
{
	SDL_SetWindowBrightness(mainWindow, bright);
}

void ModuleWindow::SetWinSize(int w, int h)
{
	SDL_SetWindowSize(app->window->mainWindow, w, h);
}
