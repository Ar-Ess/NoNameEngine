#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"

#include "../Source/External/ImGui/imgui.h"
#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl2.h"

#include <stdio.h>
#include "External/SDL/include/SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "External/SDL/include/SDL_opengl.h"
#endif

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;
	App->renderer->camera.x = App->renderer->camera.y = 0;

	return ret;
}

bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	return true;
}

update_status ModuleSceneIntro::Update()
{
	bool ret = true;

	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Poll and handle events (inputs, window resize, etc.)
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			ret = false;
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(App->mainWindow))
			ret = false;
	}

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->mainWindow);
	ImGui::NewFrame();

	bool* open;
	bool titleBar = false, scrollBar = false, menu = false, move = false, resize = false, collapse = false, 
		nav = false, background = false, bringToFront = false, docking = false, unsavedDocument = false, close = false;
	ImGuiWindowFlags window_flags = 0;
	if (titleBar)        window_flags |= ImGuiWindowFlags_NoTitleBar;
	if (scrollBar)       window_flags |= ImGuiWindowFlags_NoScrollbar;
	if (!menu)           window_flags |= ImGuiWindowFlags_MenuBar;
	if (move)            window_flags |= ImGuiWindowFlags_NoMove;
	if (resize)          window_flags |= ImGuiWindowFlags_NoResize;
	if (collapse)        window_flags |= ImGuiWindowFlags_NoCollapse;
	if (nav)             window_flags |= ImGuiWindowFlags_NoNav;
	if (background)      window_flags |= ImGuiWindowFlags_NoBackground;
	if (bringToFront)  window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;
	if (docking)         window_flags |= ImGuiWindowFlags_NoDocking;
	if (unsavedDocument)   window_flags |= ImGuiWindowFlags_UnsavedDocument;
	if (close)           open = NULL; // Don't pass our bool* to Begin

	if (!ImGui::Begin("Dear ImGui Demo", &ret, (ImGuiWindowFlags)0))
	{
		// Early out if the window is collapsed, as an optimization.
		ImGui::End();
		if (!ret) return UPDATE_STOP;
		return UPDATE_CONTINUE;
	}

	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 650, main_viewport->WorkPos.y + 20), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_FirstUseEver);

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	if (exampleWindow)
		ImGui::ShowDemoWindow(&exampleWindow);

	{
		ImGui::Begin("Main Menu", &ret);
		{
			/*ImGui::BeginMenuBar();
			{
				ImGui::BeginMenu("Menu");
				{
					if (ImGui::MenuItem("Repository")) ShellExecuteA(NULL, "open", "https://github.com/BooStarGamer/Game-Engine-1.0v", NULL, NULL, SW_SHOWNORMAL);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();*/

			if (ImGui::Button("Quit", { 100, 20 })) ret = false;
			ImGui::Checkbox("Example Window", &exampleWindow);
			ImGui::SameLine();
		}
		ImGui::End();
	}

	/* 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}*/

	/* 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}*/

	// Rendering
	ImGui::Render();
	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());

	// Update and Render additional Platform Windows
	// (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
	//  For this specific demo app we could also call SDL_GL_MakeCurrent(window, gl_context) directly)
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
		SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
	}

	SDL_GL_SwapWindow(App->mainWindow);

	if (!ret) return UPDATE_STOP;

	return UPDATE_CONTINUE;
}
