#include "EditorScene.h"

EditorScene::EditorScene(Application* App)
{
	this->app = App;
}

EditorScene::~EditorScene()
{
}

bool EditorScene::Start()
{
	return true;
}

bool EditorScene::Update()
{
	bool ret = true;

	// Quit when clicking window cross
	SDL_Event event;
	while (SDL_PollEvent(&event))
	{
		ImGui_ImplSDL2_ProcessEvent(&event);
		if (event.type == SDL_QUIT)
			ret = false;
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(app->mainWindow))
			ret = false;
	}

	// Generate new frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(app->mainWindow);
	ImGui::NewFrame();

	// GUI Implementation
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
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save File")) app->Save("NNE_Project_Saving");
				if (ImGui::MenuItem("Load File")) app->Load("NNE_Project_Saving");
				if (ImGui::MenuItem("Exit")) ret = false;
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Gui Demo"))
					exampleWindow = !exampleWindow;

				if (ImGui::MenuItem("Repository"))
					ShellExecuteA(NULL, "open", "https://github.com/BooStarGamer/Game-Engine-1.0v", NULL, NULL, SW_SHOWNORMAL);

				if (ImGui::MenuItem("Documentation"))
					ShellExecuteA(NULL, "open", "https://github.com/BooStarGamer/Game-Engine-1.0v/wiki", NULL, NULL, SW_SHOWNORMAL);

				if (ImGui::MenuItem("Download latest"))
					ShellExecuteA(NULL, "open", "https://github.com/BooStarGamer/Game-Engine-1.0v/releases", NULL, NULL, SW_SHOWNORMAL);

				if (ImGui::MenuItem("Report Bug"))
					ShellExecuteA(NULL, "open", "https://github.com/BooStarGamer/Game-Engine-1.0v/issues", NULL, NULL, SW_SHOWNORMAL);

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
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

	return ret;
}

bool EditorScene::CleanUp()
{
	bool ret = true;

	return ret;
}
