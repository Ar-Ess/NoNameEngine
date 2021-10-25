#pragma once
#include "Globals.h"
#include "External/SDL/include/SDL_opengl.h"

class Rotation
{
public:

	Rotation(float anglex, float planeXx, float planeYx, float planeZx)
	{
		angle = anglex;
		planeX = planeXx;
		planeY = planeYx;
		planeZ = planeZx;
	}

	float angle = 0.0f;
	float planeX = 0;
	float planeY = 0;
	float planeZ = 0;
};

class Cube3D
{
public:
	Cube3D(Point3D pos = {0.0f, 0.0f, 0.0f}, float s = 1, Rotation rot = { 0, 0, 0, 0 })
	{
		position = pos;
		scale = s;
		rotation = rot;

		for (int i = 0; i < 8; i++)
		{
			v[i] *= s;
			v[i] += pos;
		}
	}

	void Draw()
	{
		if (selected)
		{
			Reset();

			for (int i = 0; i < 8; i++)
			{
				v[i] *= scale;
				v[i] += position;
			}
		}

		glBegin(GL_TRIANGLES);
		// front face =================
		glVertex3f(v[1].x, v[1].y, v[1].z); // v1
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0
		glVertex3f(v[3].x, v[3].y, v[3].z); // v3

		glVertex3f(v[3].x, v[3].y, v[3].z); // v3
		glVertex3f(v[2].x, v[2].y, v[2].z); // v2
		glVertex3f(v[1].x, v[1].y, v[1].z); // v1

		// right face =================
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0
		glVertex3f(v[5].x, v[5].y, v[5].z); // v5
		glVertex3f(v[4].x, v[4].y, v[4].z); // v4

		glVertex3f(v[4].x, v[4].y, v[4].z); // v4
		glVertex3f(v[3].x, v[3].y, v[3].z); // v3
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0

		// top face ===================
		glVertex3f(v[6].x, v[6].y, v[6].z); // v6
		glVertex3f(v[5].x, v[5].y, v[5].z); // v5
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0

		glVertex3f(v[0].x, v[0].y, v[0].z); // v0
		glVertex3f(v[1].x, v[1].y, v[1].z); // v1
		glVertex3f(v[6].x, v[6].y, v[6].z); // v6

		// bottom face ===================
		glVertex3f(v[2].x, v[2].y, v[2].z); // v2
		glVertex3f(v[3].x, v[3].y, v[3].z); // v3
		glVertex3f(v[4].x, v[4].y, v[4].z); // v4

		glVertex3f(v[4].x, v[4].y, v[4].z); // v4
		glVertex3f(v[7].x, v[7].y, v[7].z); // v7
		glVertex3f(v[2].x, v[2].y, v[2].z); // v2

		// left face ===================
		glVertex3f(v[6].x, v[6].y, v[6].z); // v6
		glVertex3f(v[1].x, v[1].y, v[1].z); // v1
		glVertex3f(v[2].x, v[2].y, v[2].z); // v2

		glVertex3f(v[2].x, v[2].y, v[2].z); // v2
		glVertex3f(v[7].x, v[7].y, v[7].z); // v7
		glVertex3f(v[6].x, v[6].y, v[6].z); // v6

		// back face ===================
		glVertex3f(v[6].x, v[6].y, v[6].z); // v6
		glVertex3f(v[7].x, v[7].y, v[7].z); // v7
		glVertex3f(v[4].x, v[4].y, v[4].z); // v4

		glVertex3f(v[4].x, v[4].y, v[4].z); // v4
		glVertex3f(v[5].x, v[5].y, v[5].z); // v5
		glVertex3f(v[6].x, v[6].y, v[6].z); // v6

		glEnd();

		glRotatef(rotation.angle, rotation.planeX, rotation.planeY, rotation.planeZ);
	}

	void Reset()
	{
		v[0] = { 1.0f, 1.0f, 0.0f };
		v[1] = { 0.0f, 1.0f, 0.0f };
		v[2] = { 0.0f, 0.0f, 0.0f };
		v[3] = { 1.0f, 0.0f, 0.0f };
		v[4] = { 1.0f, 0.0f, 1.0f };
		v[5] = { 1.0f, 1.0f, 1.0f };
		v[6] = { 0.0f, 1.0f, 1.0f };
		v[7] = { 0.0f, 0.0f, 1.0f };
	}

private:
	Point3D position = { 0.0f, 0.0f, 0.0f };
	float scale = 1;
	Rotation rotation = {0, 0, 0, 0};
	bool selected = false;
	Point3D v[8] = { { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
						{ 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };
};
