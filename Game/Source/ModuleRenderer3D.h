#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include "Light.h"

#include "External/SDL/include/SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "External/SDL/include/SDL_opengl.h"
#endif

struct Light;

enum class BasicShapes
{
	CUBE = 1,
	PYRAMID,
	SPHERE,
	LINE
};

enum class GeometryView
{
	DEPTH_TEST,
	CULL_FACE,
	LIGHTING,
	COLOR_MATERIAL,
	TEXTURE_2D
};

#define MAX_LIGHTS 8

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init();
	update_status PreUpdate();
	update_status PostUpdate();
	bool Draw();
	bool DebugDraw();
	bool GuiDraw();
	bool GeometryDraw();
	bool CleanUp();

	void OnResize(int width, int height);
	void SetVSync(bool vSync);

	void ToggleGeometryView(GeometryView gV, bool active)
	{
		GLenum gViewType = GL_DEPTH_TEST;

		switch (gV)
		{
		case GeometryView::DEPTH_TEST: gViewType = GL_DEPTH_TEST;
		case GeometryView::CULL_FACE: gViewType = GL_CULL_FACE;
		case GeometryView::LIGHTING: gViewType = GL_LIGHTING;
		case GeometryView::COLOR_MATERIAL: gViewType = GL_COLOR_MATERIAL;
		case GeometryView::TEXTURE_2D: gViewType = GL_TEXTURE_2D;
		}

		if (active) glEnable(gViewType);
		else glDisable(gViewType);
	}

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};