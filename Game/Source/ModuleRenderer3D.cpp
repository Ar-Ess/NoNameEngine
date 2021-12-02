#include "Globals.h"
#include "Application.h"
#include "ModuleRenderer3D.h"
#include "ModuleCamera3D.h"
#include "Shapes3D.h"

#include "ModuleWindow.h"
#include <math.h>
#include "ModuleSceneIntro.h"
#include "External/SDL/include/SDL.h"
#include "External/ImGuizmo-master/ImGuizmo.h"
#include "physfs.h"

#pragma comment (lib, "Source/External/glew/glew32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

ModuleRenderer3D::ModuleRenderer3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModuleRenderer3D::~ModuleRenderer3D()
{}

bool ModuleRenderer3D::Init()
{
	LOG("Creating 3D Renderer context");
	bool ret = true;
	
	//Create context
	context = SDL_GL_CreateContext(app->window->mainWindow);
	if(context == NULL)
	{
		LOG("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
		ret = false;
	}
	
	if(ret == true)
	{
		//Use Vsync
		if(VSYNC && SDL_GL_SetSwapInterval(1) < 0)
			//LOG("Warning: Unable to set VSync! SDL Error: %s\n", SDL_GetError());

		//Initialize Projection Matrix
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		GLenum error = glewInit();
		if (error != GLEW_OK)
		{
			ret = false;
		}

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}

		//Initialize Modelview Matrix
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		glClearDepth(1.0f);
		
		//Initialize clear color
		glClearColor(0.f, 0.f, 0.f, 1.f);

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		//Check for error
		error = glGetError();
		if(error != GL_NO_ERROR)
		{
			//LOG("Error initializing OpenGL! %s\n", gluErrorString(error));
			ret = false;
		}
		
		GLfloat LightModelAmbient[] = {0.0f, 0.0f, 0.0f, 1.0f};
		glLightModelfv(GL_LIGHT_MODEL_AMBIENT, LightModelAmbient);
		
		lights[0].ref = GL_LIGHT0;
		lights[0].ambient.Set(0.25f, 0.25f, 0.25f, 1.0f);
		lights[0].diffuse.Set(0.75f, 0.75f, 0.75f, 1.0f);
		lights[0].SetPos(0.0f, 0.0f, 2.5f);
		lights[0].Init();
		
		GLfloat MaterialAmbient[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, MaterialAmbient);

		GLfloat MaterialDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, MaterialDiffuse);
		
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		lights[0].Active(true);
		glEnable(GL_LIGHTING);
		glEnable(GL_COLOR_MATERIAL);
		glEnable(GL_TEXTURE_2D);
	}

	// Projection matrix for
	OnResize(SCREEN_WIDTH, SCREEN_HEIGHT);

	return ret;
}

update_status ModuleRenderer3D::PreUpdate()
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
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(app->window->mainWindow);
	ImGui::NewFrame();
	ImGuizmo::BeginFrame();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixf(app->camera->GetViewMatrix()); // PROBLEM: HOPEFULLY IS THIS. BUT I DON'T KNOW HOW TO SOLVE IT. IF I CHANGE IT; CAMERA WON?T WORK ANYMORE...

	if (!ret) return UPDATE_STOP;
	return UPDATE_CONTINUE;
}

update_status ModuleRenderer3D::PostUpdate()
{

	DebugDraw();
	Draw();

	SDL_GL_SwapWindow(app->window->mainWindow);
	return UPDATE_CONTINUE;
}

bool ModuleRenderer3D::Draw()
{
	bool ret = true;

	ret = GeometryDraw();
	ret = GuiDraw();
	
	return ret;
}

bool ModuleRenderer3D::DebugDraw()
{
	return true;
}

bool ModuleRenderer3D::GuiDraw()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Render();

	glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
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

	return true;
}

bool ModuleRenderer3D::GeometryDraw()
{
	bool ret = true;
	lights[0].SetPos(app->camera->position.x, app->camera->position.y, app->camera->position.z);

	for (int i = 0; i < app->scene->shapes.size(); i++)
	{
		glPushMatrix();
		ret = app->scene->shapes[i]->Draw();
		glPopMatrix();
	}

	for (uint i = 0; i < MAX_LIGHTS; ++i)
		lights[i].Render();

	return ret;
}

bool ModuleRenderer3D::CleanUp()
{
	LOG("Destroying 3D Renderer");

	SDL_GL_DeleteContext(context);

	return true;
}

void ModuleRenderer3D::OnResize(int width, int height)
{
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	ProjectionMatrix = perspective(60.0f, (float)width / (float)height, 0.125f, 512.0f);
	glLoadMatrixf(&ProjectionMatrix);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ModuleRenderer3D::SetVSync(bool vSync)
{
	if (vSync)
	{
		SDL_GL_SetSwapInterval(-1);
	}
	else if (!vSync)
	{
		SDL_GL_SetSwapInterval(0);
	}
}
