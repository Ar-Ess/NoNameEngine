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
		if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE && event.window.windowID == SDL_GetWindowID(app->window->mainWindow))
			ret = false;
	}

	// Generate new frame
	ImGui_ImplOpenGL2_NewFrame();
	ImGui_ImplSDL2_NewFrame(app->window->mainWindow);
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
	if (open)
	{
		if (ImGui::Begin("Output Log", &open))
		{
			for (int i = 0; i < GetOutputSize(); i++)
			{
				ImGui::Text(GetOutputText(i).c_str());
			}
			ImGui::End();
		}
	}

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
				AddSpacing(0);
				ImGui::Text("PROJECT INFORMATION");
				AddSpacing(1);
				ImGui::Text("Project Name");
				ImGui::InputText("New Project", &projectName);
				AddSpacing(0);
				ImGui::Text("Developer Team");
				ImGui::InputText("Team Name", &teamName);
				AddSpacing(1);

				AddSeparator(2);
				
				AddSpacing(0);
				ImGui::Text("ENGINE PERFORMANCE");
				AddSpacing(1);
				ImGui::Text("Framerate");
				ImGui::SliderInt(" FPS", &app->fps, 5, 60, "FPS: %d");
				AddSpacing(0);

				char title[25];
				framerate = app->fpsLog[app->fpsLog.size() - 2];
				sprintf_s(title, 25, "Framerate %.1f", framerate);
				ImGui::Text("Framerate Graph");
				ImGui::PlotHistogram("##framerate", &app->fpsLog[0], app->fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(307, 100));
				ImGui::SliderInt(" FBL", &app->frameBarLimit, 30, 80, "Bars: %d");

				AddSpacing(0);

				milliseconds = app->msLog[app->msLog.size() - 2];
				sprintf_s(title, 25, "Milliseconds %0.1f", milliseconds);
				ImGui::Text("Milliseconds Graph");
				ImGui::PlotHistogram("##milliseconds", &app->msLog[0], app->msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(307, 100));
				ImGui::SliderInt(" MSG", &app->msBarLimit, 30, 80, "Bars: %d");

				AddSpacing(1);

				AddSeparator(2);
			}
			if (ImGui::CollapsingHeader("Window"))
			{
				Point prev = wSize;
				int prevProportion = wSizeProportion;
				float prevBright = brightLevel;

				AddSpacing(0);
				ImGui::Text("Brightness");
				ImGui::SliderFloat("B", &brightLevel, 0.0f, 1.0f, "Value: %.3f");
				if (prevBright != brightLevel) app->window->SetWinBrightness(brightLevel);

				AddSpacing(0);
				if (ImGui::Checkbox("Keep Proportion", &wKeepProportion)) wKeepProportion = wKeepProportion;

				if (!wKeepProportion)
				{
					AddSpacing(0);
					ImGui::Text("Width");
					ImGui::SliderInt("W", &wSize.x, 1, SCREEN_WIDTH, "%d");

					AddSpacing(0);
					ImGui::Text("Height");
					ImGui::SliderInt("H", &wSize.y, 1, SCREEN_HEIGHT, "%d");

					if (wSize != prev) app->window->SetWinSize((int)wSize.x, (int)wSize.y);
				}
				else
				{
					AddSpacing(0);
					ImGui::Text("Width / Height");
					ImGui::SliderInt("W", &wSizeProportion, 1, 100, "%d");

					if (wSizeProportion != prevProportion) app->window->SetWinSize(int(floor(SCREEN_WIDTH * wSizeProportion / 100)), int(floor(SCREEN_HEIGHT * wSizeProportion / 100)));
				}

				AddSpacing(1);
				if (ImGui::Checkbox(" VSync", &wVSync)) app->renderer->SetVSync(wVSync);

				AddSpacing(0);
				if (ImGui::Checkbox(" Full Screen    ", &wFullScreen)) app->window->SetWinFullScreen(wFullScreen);
				ImGui::SameLine();
				if (ImGui::Checkbox(" Resizable", &wResizable)) app->SaveRestartPropierties();

				AddSpacing(0);
				if (ImGui::Checkbox(" Borderless     ", &wBorderless)) app->window->SetWinBorders(wBorderless);
				ImGui::SameLine();
				if (ImGui::Checkbox(" Full Desktop", &wFullDesktop)) app->window->SetWinFullScreen(wFullDesktop);

				AddSpacing(1);
			}
			if (ImGui::CollapsingHeader("Hardware"))
			{
				AddSpacing(0);
				SDL_version v;
				SDL_GetVersion(&v);
				ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(YELLOW), "%d.%d.%d", v.major, v.minor, v.patch);
				AddSpacing(0);
				AddSeparator(2);
				AddSpacing(0);
				ImGui::Text("CPUs: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(YELLOW), "%d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
				AddSpacing(0);
				ImGui::Text("System RAM: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(YELLOW), "%d", SDL_GetSystemRAM);
				AddSpacing(0);
				ImGui::Text("Caps: "); ImGui::SameLine(); 
				{
					if (SDL_Has3DNow()) ImGui::TextColored(ImVec4(YELLOW), "3DNow"); ImGui::SameLine();
					if (SDL_HasAltiVec()) ImGui::TextColored(ImVec4(YELLOW), "AltiVec"); ImGui::SameLine();
					if (SDL_HasAVX()) ImGui::TextColored(ImVec4(YELLOW), "AVX"); ImGui::SameLine();
					if (SDL_HasAVX2()) ImGui::TextColored(ImVec4(YELLOW), "AVX2"); ImGui::SameLine();
					if (SDL_HasMMX()) ImGui::TextColored(ImVec4(YELLOW), "MMX"); ImGui::SameLine();
					if (SDL_HasRDTSC()) ImGui::TextColored(ImVec4(YELLOW), "RDTSC");
					if (SDL_HasSSE()) ImGui::TextColored(ImVec4(YELLOW), "SSE"); ImGui::SameLine();
					if (SDL_HasSSE2()) ImGui::TextColored(ImVec4(YELLOW), "SSE2"); ImGui::SameLine();
					if (SDL_HasSSE3()) ImGui::TextColored(ImVec4(YELLOW), "SSE3"); ImGui::SameLine();
					if (SDL_HasSSE41()) ImGui::TextColored(ImVec4(YELLOW), "SSE41"); ImGui::SameLine();
					if (SDL_HasSSE42()) ImGui::TextColored(ImVec4(YELLOW), "SSE42");
				}
				AddSpacing(0);
				AddSeparator(2);
			}
			ImGui::End();
		}
	}

	return open;
}