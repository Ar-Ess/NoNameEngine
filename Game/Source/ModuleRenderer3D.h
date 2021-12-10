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
#include "External/MathGeoLib/Geometry/AABB.h"
#include "External/MathGeoLib/Geometry/Frustum.h"
#include "Model.h"
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
		case GeometryView::DEPTH_TEST: gViewType = GL_DEPTH_TEST; break;
		case GeometryView::CULL_FACE: gViewType = GL_CULL_FACE; break;
		case GeometryView::LIGHTING: gViewType = GL_LIGHTING; break;
		case GeometryView::COLOR_MATERIAL: gViewType = GL_COLOR_MATERIAL; break;
		case GeometryView::TEXTURE_2D: gViewType = GL_TEXTURE_2D; break;
		}

		if (active) glEnable(gViewType);
		else glDisable(gViewType);
	}

	void ModuleRenderer3D::DrawFrustum(Frustum& frustum)
	{
		float3 corners[8];
		frustum.GetCornerPoints(corners);
		DrawWireCube(corners, Blue);
	}

	void ModuleRenderer3D::DrawWireCube(float3* vertex, Color color)
	{
		glBegin(GL_LINES);

		glColor4f(color.r, color.g, color.b, color.a);

		//Between-planes right
		glVertex3fv((GLfloat*)&vertex[1]);
		glVertex3fv((GLfloat*)&vertex[5]);
		glVertex3fv((GLfloat*)&vertex[7]);
		glVertex3fv((GLfloat*)&vertex[3]);

		//Between-planes left
		glVertex3fv((GLfloat*)&vertex[4]);
		glVertex3fv((GLfloat*)&vertex[0]);
		glVertex3fv((GLfloat*)&vertex[2]);
		glVertex3fv((GLfloat*)&vertex[6]);

		//Far plane horizontal 
		glVertex3fv((GLfloat*)&vertex[5]);
		glVertex3fv((GLfloat*)&vertex[4]);
		glVertex3fv((GLfloat*)&vertex[6]);
		glVertex3fv((GLfloat*)&vertex[7]);

		//Near plane horizontal
		glVertex3fv((GLfloat*)&vertex[0]);
		glVertex3fv((GLfloat*)&vertex[1]);
		glVertex3fv((GLfloat*)&vertex[3]);
		glVertex3fv((GLfloat*)&vertex[2]);

		//Near plane vertical  
		glVertex3fv((GLfloat*)&vertex[1]);
		glVertex3fv((GLfloat*)&vertex[3]);
		glVertex3fv((GLfloat*)&vertex[0]);
		glVertex3fv((GLfloat*)&vertex[2]);

		//Far plane vertical   
		glVertex3fv((GLfloat*)&vertex[5]);
		glVertex3fv((GLfloat*)&vertex[7]);
		glVertex3fv((GLfloat*)&vertex[4]);
		glVertex3fv((GLfloat*)&vertex[6]);

		glEnd();
	}

	void ModuleRenderer3D::DrawBoundingBox(Shape3D* shape)
	{
		if (shape->GetShapeType() == ShapeType::MODEL3D)
		{
			float3 corners[8];
			Model* m = (Model*)shape;
			m->box.GetCornerPoints(corners);
			DrawWireCube(corners, Green);
		}
		
	}

public:

	Light lights[MAX_LIGHTS] = {};
	SDL_GLContext context = {};
	mat3x3 NormalMatrix = {};
	mat4x4 ModelMatrix = {}, ViewMatrix = {}, ProjectionMatrix = {};

private:

	Model model;
};