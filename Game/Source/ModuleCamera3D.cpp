#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "External/SDL/include/SDL.h"
#include "ModuleSceneIntro.h"
#include "glmath.h"
#include "AudioSystem.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled, AudioSystem* audioSystem) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	audio = audioSystem;

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	//position = vec3(-2.0f, 5.0f, 12.0f);
	position = vec3(0.0f, 0.0f, -10.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);

	// Create our frustum using the MathGeoLib functions
	frustum.SetPos(float3(0.f, 0.f, -10.f)); // Frustum position
	frustum.SetViewPlaneDistances(0.1f, 1000.0f); // Frustum distances of the near plane and the far plane
	frustum.SetPerspective(1.f, 1.f); // 1,1 perspective to get a pyramid-like frustum
	frustum.SetFrame(float3(100.f, 100.f, 100.f), float3(0.f, 0.f, 1), float3(0.f, 1.f, 0.f));
	frustum.SetHorizontalFovAndAspectRatio(frustum.HorizontalFov(), 1.77f); // Definition of the hFov
	frustum.SetVerticalFovAndAspectRatio(frustum.VerticalFov(), 1.77f); // definition of the vFov
	frustum.GetPlanes(planes);

	/*ray = float3(app->input->GetMouseX(), app->input->GetMouseY(), 0.f);*/
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	frustum.SetPos(float3(0.f, 0.f, -10.f));
	audio->CreateListener(position.x, position.y, position.z);
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

update_status ModuleCamera3D::PostUpdate()
{
	
	if (app->scene->GetOnWindow() || app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || (app->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT)) return UPDATE_CONTINUE;
	
	vec3 newPos(0, 0, 0);
	vec3 newPosition(0, 0, 0);
	int wheelZV = app->input->GetMouseWheel();
	int wheelZH = app->input->GetMouseZH();
	int dx = -app->input->GetMouseXMotion();
	int dy = -app->input->GetMouseYMotion();
	bool shift = (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT);
	bool lclick = (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT);
	float frameSpeed = speed;
	if (app->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT) frameSpeed *= 2;

	ray = NormaliseRay(ray);
	CreateVec4(ray);
	RayCoordinates(ray4);


	// Trackpad movement
	if (wheelZV != 0)
	{
		if (shift)
		{
			if (wheelZV > 0) newPos += Y * frameSpeed;
			if (wheelZV < 0) newPos -= Y * frameSpeed;
		}
		else
		{
			if (wheelZV > 0) newPos -= Z * frameSpeed;
			if (wheelZV < 0) newPos += Z * frameSpeed;
		}

	}
	if (wheelZH != 0)
	{
		if (wheelZH > 0) newPos += X * frameSpeed;
		if (wheelZH < 0) newPos -= X * frameSpeed;
	}

	// Mouse movement
	if (lclick)
	{
		if (dx != 0 && !shift)
		{
			if (dx > 0) newPos += X * frameSpeed * sens;
			if (dx < 0) newPos -= X * frameSpeed * sens;

		}
		if (dy != 0)
		{
			if (shift)
			{
				if (dy > 0) newPosition += Z * frameSpeed * sens;
				if (dy < 0) newPosition -= Z * frameSpeed * sens;
			}
			else
			{
				if (dy > 0) newPos -= Y * frameSpeed * sens;
				if (dy < 0) newPos += Y * frameSpeed * sens;
			}
		}
	}

	// Arrow movement
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		if (shift) newPosition.y += frameSpeed;
		else
		{
			newPosition -= Z * frameSpeed;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		if (shift) newPosition.y -= frameSpeed;
		else
		{
			newPosition += Z * frameSpeed;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPosition -= X * frameSpeed;
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPosition += X * frameSpeed;

	// Position setting
	position += newPos;
	position += newPosition;
	lookPoint += newPosition;

	if (app->input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT) lookPoint = Focus();

	frustum.SetPos(frustum.Pos() + float3(newPosition.x, newPosition.y, newPosition.z));
	frustum.SetPos(frustum.Pos() + float3(newPos.x, newPos.y, newPos.z));
	frustum.GetPlanes(planes);
	// Recalculate matrix
	LookAt(vec3{ lookPoint.x, lookPoint.y, lookPoint.z });
	CalculateViewMatrix();

	audio->CreateListener(position.x, position.y, position.z); // We constantly update the position of the listener with the camera position

	return UPDATE_CONTINUE;
}

void ModuleCamera3D::Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference)
{
	this->position = Position;
	this->reference = Reference;

	Z = normalize(Position - Reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	if(!RotateAroundReference)
	{
		this->reference = this->position;
		this->position += Z * 0.05f;
	}

	CalculateViewMatrix();
}

void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	reference = Spot;

	Z = normalize(position - reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

void ModuleCamera3D::Move(const vec3 &Movement)
{
	position += Movement;
	reference += Movement;

	CalculateViewMatrix();
}

Point3D ModuleCamera3D::Focus()
{
	int size = app->scene->shapes.size();

	if (size == 1) return Point3D{ 0, 0, 0 };

	if (size == 2) return app->scene->shapes[1]->GetPosition();

	Point3D minor = { 0, 0, 0 };
	bool first = true;

	for (unsigned int i = 1; i < app->scene->shapes.size(); i++)
	{
		Point3D pos = app->scene->shapes[i]->GetPosition();
		if (first)
		{
			minor = pos;
			first = false;
		}
		else
		{
			if (minor > pos) minor = pos;
		}
	}

	return minor;
}

float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, position), -dot(Y, position), -dot(Z, position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}

vec3 ModuleCamera3D::NormaliseRay(vec3 ray3)
{
	float x = (2.f * app->input->GetMouseX() / app->window->GetWindowWidth() - 1.f);
	float y = 1.f - (2.f * app->input->GetMouseY()) / app->window->GetWindowHeight();
	float z = 1.0f;
	ray3 = vec3(x, y, z);

	return ray3;
}

void ModuleCamera3D::CreateVec4(vec3 ray3)
{
	ray4 = vec4(ray3.x, ray3.y, -1.0, 1.0);
}

void ModuleCamera3D::RayCoordinates(vec4 ray4)
{
	vec4 rayEye = inverse(ViewMatrix) * ray4;
	ray4 = vec4(rayEye.x, rayEye.y, -1.f, 0.f);

	vec3 ray3 = ((inverse(ViewMatrix) * rayEye).x, (inverse(ViewMatrix) * rayEye).y, (inverse(ViewMatrix) * rayEye).z);
	ray3 = NormaliseRay(ray3);
	
}

void ModuleCamera3D::SelectObject(Shape3D* shape)
{
	float2 mousePos = float2(float(app->input->GetMouseX()), float(app->window->GetWindowHeight() - app->input->GetMouseY()));

	float mouseNormalisedX = mousePos.x / app->window->GetWindowWidth();
	float mouseNormalisedY = mousePos.y / app->window->GetWindowHeight();

	mouseNormalisedX = float((mouseNormalisedX - 0.5) / 0.5);
	mouseNormalisedY = float((mouseNormalisedY - 0.5) / 0.5);

	LineSegment selectRay;
	selectRay = frustum.UnProjectLineSegment(mouseNormalisedX, mouseNormalisedY);


	CheckRaycast(&selectRay, shape);
}

void ModuleCamera3D::CheckRaycast(LineSegment* ray, Shape3D* shape)
{
	bool hovered = false;
	Model* object = (Model*)shape;
	std::map<float, Shape3D*> hits;

	// We check if the mouse is hovering the shape
	if (ray->Intersects(object->box))
	{
		hovered = true;
	}

	// We check if the mouse is clicking the shape
	if (hovered && app->input->GetMouseButton(SDL_BUTTON_LEFT) && !app->scene->GetOnWindow())
	{
		shape->selected = true;
	}

	// We check if the mouse is clicking outside of the shape
	if (!hovered && app->input->GetMouseButton(SDL_BUTTON_LEFT) && !app->scene->GetOnWindow())
	{
		shape->selected = false;
	}

	// We check if we are clicking any option in the Hierarchy of the selected object
	if (shape->selected == true && app->scene->GetOnWindow())
	{
		shape->selected = true;
	}



	/*if (objectFound)
		app->scene->SetSelectedObject((*hits.begin()).second);*/
	//else
		//App->scene->SetSelectedObject(nullptr);

}

//Frustum ModuleCamera3D::CreateFrustum(ModuleCamera3D cam, float aspect, float fovY, float zNear, float zFar)
//{
//	Frustum frustum;
//	const float halfVSide = zFar * tanf(fovY * 0.5f);
//	const float halfHSide = halfVSide * aspect;
//	const vec3 frontMultFar = zFar *lookPoint.z;
//
//	frustum.nearPlane = { position + zNear * lookPoint.z, lookPoint.z };
//	frustum.farPlane = { position + frontMultFar, -lookPoint.z };
//	/*frustum.rightPlane = {position, cross()}*/
//	return frustum;
//}
