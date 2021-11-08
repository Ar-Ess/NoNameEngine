#include "Globals.h"
#include "Application.h"
#include "ModuleCamera3D.h"
#include "ModuleInput.h"
#include "External/SDL/include/SDL.h"
#include "ModuleSceneIntro.h"

ModuleCamera3D::ModuleCamera3D(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	CalculateViewMatrix();

	X = vec3(1.0f, 0.0f, 0.0f);
	Y = vec3(0.0f, 1.0f, 0.0f);
	Z = vec3(0.0f, 0.0f, 1.0f);

	position = vec3(-2.0f, 5.0f, 12.0f);
	reference = vec3(0.0f, 0.0f, 0.0f);
}

ModuleCamera3D::~ModuleCamera3D()
{}

bool ModuleCamera3D::Start()
{
	LOG("Setting up the camera");
	bool ret = true;

	return ret;
}

bool ModuleCamera3D::CleanUp()
{
	LOG("Cleaning camera");

	return true;
}

update_status ModuleCamera3D::Update()
{
	//if (app->scene->GetWindowState(Windows::CONFIG_W) || app->scene->GetWindowState(Windows::DEMO_W) || app->scene->GetWindowState(Windows::OUTPUT_W) || app->scene->GetWindowState(Windows::ABOUT_W)) return UPDATE_CONTINUE;
	if (app->scene->GetOnWindow()) return UPDATE_CONTINUE;
	
	vec3 newPos(0, 0, 0);
	vec3 newPosition(0, 0, 0);
	int wheelZV = app->input->GetMouseWheel();
	int wheelZH = app->input->GetMouseZH();
	int dx = -app->input->GetMouseXMotion();
	int dy = -app->input->GetMouseYMotion();
	bool shift = (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT);

	// Trackpad movement
	if (wheelZV != 0)
	{
		if (shift)
		{
			if (wheelZV > 0) newPos -= Z * speed;
			if (wheelZV < 0) newPos += Z * speed;
		}
		else
		{
			if (wheelZV > 0) newPos += Y * speed;
			if (wheelZV < 0) newPos -= Y * speed;
		}

	}
	if (wheelZH != 0)
	{
		if (wheelZH > 0) newPos += X * speed;
		if (wheelZH < 0) newPos -= X * speed;
	}

	// Mouse movement
	if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		if (dx != 0 && !shift)
		{
			if (dx > 0) newPos += X * speed * sens;
			if (dx < 0) newPos -= X * speed * sens;

		}
		if (dy != 0)
		{
			if (shift)
			{
				if (dy > 0) newPosition += Z * speed * sens;
				if (dy < 0) newPosition -= Z * speed * sens;
			}
			else
			{
				if (dy > 0) newPos -= Y * speed * sens;
				if (dy < 0) newPos += Y * speed * sens;
			}
		}
	}

	// Arrow movement
	if (app->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
	{
		if (shift) newPosition.y += speed;
		else
		{
			newPosition -= Z * speed;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
	{
		if (shift) newPosition.y -= speed;
		else
		{
			newPosition += Z * speed;
		}
	}
	if (app->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) newPosition -= X * speed;
	if (app->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) newPosition += X * speed;

	// Position setting
	position += newPos;
	position += newPosition;
	lookPoint += newPosition;

	// Recalculate matrix
	LookAt(vec3{ lookPoint.x, lookPoint.y, lookPoint.z });
	CalculateViewMatrix();

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

float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, position), -dot(Y, position), -dot(Z, position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}