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

	position = vec3(0.0f, 0.0f, 5.0f);
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

	vec3 newPos(0, 0, 0);
	vec3 newPosition(0, 0, 0);
	float speed = 0.5f;
	int wheelZV = app->input->GetMouseZ();
	int wheelZH = app->input->GetMouseZH();
	int dx = -app->input->GetMouseXMotion();
	int dy = -app->input->GetMouseYMotion();
	bool shift = (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT);
	float sens = 0.2f;

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

	// Mouse motion
	/*if (app->input->GetMouseButton(SDL_BUTTON_LEFT) == KEY_REPEAT)
	{
		int dx = -app->input->GetMouseXMotion();
		int dy = -app->input->GetMouseYMotion();
		bool move = (dx != 0 || dy != 0);

		if (move)
		{
			position -= reference;
			if (dx != 0)
			{
				float DeltaX = (float)dx * Sensitivity;

				X = rotate(X, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Y = rotate(Y, DeltaX, vec3(0.0f, 1.0f, 0.0f));
				Z = rotate(Z, DeltaX, vec3(0.0f, 1.0f, 0.0f));
			}
			if (dy != 0)
			{
				float DeltaY = (float)dy * Sensitivity;

				Y = rotate(Y, DeltaY, X);
				Z = rotate(Z, DeltaY, X);

				if (Y.y < 0.0f)
				{
					Z = vec3(0.0f, Z.y > 0.0f ? 1.0f : -1.0f, 0.0f);
					Y = cross(Z, X);
				}
			}
			position = reference + Z * length(position);
		}
	}
	*/

	// Recalculate matrix
	LookAt(vec3{ lookPoint.x, lookPoint.y, lookPoint.z });
	CalculateViewMatrix();

	return UPDATE_CONTINUE;
}

// -----------------------------------------------------------------
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

// -----------------------------------------------------------------
void ModuleCamera3D::LookAt( const vec3 &Spot)
{
	reference = Spot;

	Z = normalize(position - reference);
	X = normalize(cross(vec3(0.0f, 1.0f, 0.0f), Z));
	Y = cross(Z, X);

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
void ModuleCamera3D::Move(const vec3 &Movement)
{
	position += Movement;
	reference += Movement;

	CalculateViewMatrix();
}

// -----------------------------------------------------------------
float* ModuleCamera3D::GetViewMatrix()
{
	return &ViewMatrix;
}

// -----------------------------------------------------------------
void ModuleCamera3D::CalculateViewMatrix()
{
	ViewMatrix = mat4x4(X.x, Y.x, Z.x, 0.0f, X.y, Y.y, Z.y, 0.0f, X.z, Y.z, Z.z, 0.0f, -dot(X, position), -dot(Y, position), -dot(Z, position), 1.0f);
	ViewMatrixInverse = inverse(ViewMatrix);
}