#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleWindow.h"

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

	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(App->mainWindow);
	ImGui::NewFrame();

	if (exampleWindow) ImGui::ShowDemoWindow(&exampleWindow);

	//{ // INITIAL WINDOW TRASHCODE
	//	ImGui::Begin("Main Menu", &ret);
	//	{
	//		if (ImGui::Button("Quit", { 100, 20 })) ret = false;
	//		ImGui::Checkbox("Example Window", &exampleWindow);
	//		if (ImGui::MenuItem("Repository")) ShellExecuteA(NULL, "open", "https://github.com/BooStarGamer/Game-Engine-1.0v", NULL, NULL, SW_SHOWNORMAL);
	//		ImGui::SameLine();
	//	}
	//	ImGui::End();
	//}

	{ // MAIN MENU BAR
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::MenuItem("Exit")) ret = false;
			if (ImGui::MenuItem("Example Window")) exampleWindow = !exampleWindow;
			if (ImGui::MenuItem("Repository")) ShellExecuteA(NULL, "open", "https://github.com/BooStarGamer/Game-Engine-1.0v", NULL, NULL, SW_SHOWNORMAL);
		}
		ImGui::EndMainMenuBar();
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

	if (!ret) return UPDATE_STOP;
	return UPDATE_CONTINUE;
}
