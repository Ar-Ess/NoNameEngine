#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "External/SDL/include/SDL.h"

#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl2.h"

ModuleWindow::ModuleWindow(Application* app, bool start_enabled) : Module(app, start_enabled)
{

}

ModuleWindow::~ModuleWindow()
{
}

bool ModuleWindow::Init()
{
	bool ret = true;

	// Create window with graphics context
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_WindowFlags wFlags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);
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
	ImGui_ImplSDL2_InitForOpenGL(this->mainWindow, gl_context);
	ImGui_ImplOpenGL2_Init();

	return ret;
}

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

void ModuleWindow::SetWinResizable(bool resizable)
{

}

void ModuleWindow::SetWinBorders(bool border)
{

}
