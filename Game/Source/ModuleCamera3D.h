#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"

class ModuleCamera3D : public Module
{
public:
	ModuleCamera3D(Application* app, bool start_enabled = true);
	~ModuleCamera3D();

	bool Start();
	update_status PostUpdate();
	bool CleanUp();

	void Look(const vec3 &Position, const vec3 &Reference, bool RotateAroundReference = false);
	void LookAt(const vec3 &Spot);
	void Move(const vec3 &Movement);
	Point3D Focus();
	float* GetViewMatrix();
	Point3D GetLookPoint()
	{
		return lookPoint;
	}
	void SetLookPoint(Point3D lP) { lookPoint = lP; }
	float* GetSpeed()
	{
		return &speed;
	}
	float* GetSensitivity()
	{
		return &sens;
	}
	void SetSpeed(float s)
	{
		speed = s;
	}
	void SetSensitivity(float s)
	{
		sens = s;
	}

	void CalculateViewMatrix();

public:
	
	vec3 X = {}, Y = {}, Z = {}, position = {}, reference = {};
	float speed = 0.5f;
	float sens = 0.3f;

private:

	mat4x4 ViewMatrix = {}, ViewMatrixInverse = {};
	Point3D lookPoint = {};
};