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

	// GUI DEMO WINDOW
	if (demoWindow) ImGui::ShowDemoWindow(&demoWindow);
	
	// MAIN MENU BAR
	{
		ImGui::BeginMainMenuBar();
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save File")) 
					app->Save("NNE_Project_Saving", FileContent::PROJECT);

				if (ImGui::MenuItem("Load File")) 
					app->Load("NNE_Project_Saving", FileContent::PROJECT);

				if (ImGui::MenuItem("Configuration"))
					configWindow = !configWindow;

				if (ImGui::MenuItem("Exit")) 
					ret = false;

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Output Log"))
					outputWindow = !outputWindow;

				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Help"))
			{
				if (ImGui::MenuItem("Gui Demo"))
					demoWindow = !demoWindow;

				if (ImGui::MenuItem("Repository"))
					LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v");

				if (ImGui::MenuItem("Documentation"))
					LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v/wiki");

				if (ImGui::MenuItem("Download latest"))
					LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v/releases");

				if (ImGui::MenuItem("Report Bug"))
					LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v/issues");

				if (ImGui::MenuItem("About"))
					aboutPopup = !aboutPopup;

				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}
	}

	aboutPopup = ShowAboutWindow(aboutPopup);
	outputWindow = ShowOutputWindow(outputWindow);
	configWindow = ShowConfigWindow(configWindow);

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

	return ret;
}

bool EditorScene::CleanUp()
{
	bool ret = true;

	return ret;
}

bool EditorScene::ShowAboutWindow(bool open)
{
	if (open)
	{
		ImGui::OpenPopup("About");

		if (ImGui::BeginPopupModal("About", &open))
		{
			ImGui::Text("Hello");
			ImGui::EndPopup();
		}
	}

	return open;
}

bool EditorScene::ShowOutputWindow(bool open)
{
	return open;
}

bool EditorScene::ShowConfigWindow(bool open)
{
	if (open)
	{
		if (ImGui::Begin("Configuration", &open))
		{
			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Save"))
					app->Save("NNE_Preferences_Saving", FileContent::CONFIG_PREFERENCES);

				if (ImGui::MenuItem("Load"))
					app->Load("NNE_Preferences_Saving", FileContent::CONFIG_PREFERENCES);

				if (ImGui::MenuItem("Default"))
					app->Load("NNE_Preferences_Default", FileContent::CONFIG_PREFERENCES);

				ImGui::EndMenu();
			}
			if (ImGui::CollapsingHeader("Application"))
			{
				ImGui::InputText(" Project Name", &projectName);

				ImGui::InputText(" Developer Team", &teamName);

				ImGui::SliderInt("Frame Graph Bars", &app->frameBarLimit, 30, 80, "Bars: %d");

				char title[25];
				framerate = app->fpsLog[app->fpsLog.size() - 2];
				sprintf_s(title, 25, "Framerate %.1f", framerate);
				ImGui::PlotHistogram("##framerate", &app->fpsLog[0], app->fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(307, 100));

				ImGui::SliderInt("Ms Graph Bars", &app->msBarLimit, 30, 80, "Bars: %d");

				milliseconds = app->msLog[app->msLog.size() - 2];
				sprintf_s(title, 25, "Milliseconds %0.1f", milliseconds);
				ImGui::PlotHistogram("##milliseconds", &app->msLog[0], app->msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(307, 100));
			}
			ImGui::End();
		}
	}

	return open;
}
