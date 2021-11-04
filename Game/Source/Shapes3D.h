#pragma once
#include "Globals.h"

#include "External/glew/glew.h"
#include "External/SDL/include/SDL_opengl.h"
#include <gl/GL.h>

#pragma comment (lib, "Source/External/glew/glew32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

struct Point3D;

class Shape3D
{
public:
	virtual bool Draw() { return true; }

	bool axis = false;
	bool edges = true;
	bool normals = false;

protected:
	Shape3D(Point3D pos, float s, Rotation rot)
	{
		position = pos;
		rotation = rot;
		scale = s;
		axis = false;
		selected = false;
	}

	void DrawAxis()
	{
		// Draw Axis Grid
		glLineWidth(2.0f);

		glBegin(GL_LINES);

		glTranslatef(position.x, position.y, position.z);

		glColor4f(1.0f, 0.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 0.1f, 0.0f); glVertex3f(1.1f, -0.1f, 0.0f);
		glVertex3f(1.1f, 0.1f, 0.0f); glVertex3f(1.0f, -0.1f, 0.0f);

		glColor4f(0.0f, 1.0f, 0.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 1.0f, 0.0f);
		glVertex3f(-0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.05f, 1.25f, 0.0f); glVertex3f(0.0f, 1.15f, 0.0f);
		glVertex3f(0.0f, 1.15f, 0.0f); glVertex3f(0.0f, 1.05f, 0.0f);

		glColor4f(0.0f, 0.0f, 1.0f, 1.0f);

		glVertex3f(0.0f, 0.0f, 0.0f); glVertex3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.1f, 1.05f); glVertex3f(0.05f, 0.1f, 1.05f);
		glVertex3f(0.05f, 0.1f, 1.05f); glVertex3f(-0.05f, -0.1f, 1.05f);
		glVertex3f(-0.05f, -0.1f, 1.05f); glVertex3f(0.05f, -0.1f, 1.05f);

		glEnd();

		glLineWidth(1.0f);
	}

	Point3D position = { 0, 0, 0 };
	Rotation rotation = { 0, 0, 0, 0};
	float scale = 1;
	bool selected = false;
};

class Cube3D : public Shape3D
{
public:
	Cube3D(Point3D pos = {0.0f, 0.0f, 0.0f}, float s = 1, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot)
	{
		for (int i = 0; i < 24; i++)
		{
			float p[3] = {pos.x, pos.y, pos.z};
			vertex[i] *= s;
			vertex[i] += p[i % 3];

			if (i < 8)
			{
				v[i] *= s;
				v[i] += pos;
			}
		}
	}

	bool Draw()
	{
		glColor3f(255, 255, 255);

		glGenBuffers(1, &vBuffer);
		glGenBuffers(1, &iBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);

		glRotatef(rotation.angle, rotation.planeX, rotation.planeY, rotation.planeZ);

		if (axis) DrawAxis();

		return true;
	}

	void DrawDirectMode()
	{
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

private:
	GLushort index[36] = { 
	0,1,2,
	0,2,3,
	1,5,6,
	1,6,2,
	0,5,1,
	0,4,5,
	0,7,4,
	0,3,7,
	4,7,6,
	4,6,5,
	3,2,6,
	3,6,7 };
	GLfloat vertex[24] = { 
	0.0f,0.0f,0.0f, // 0
	1.0f,0.0f,0.0f, // 1
	1.0f,1.0f,0.0f, // 2
	0.0f,1.0f,0.0f, // 3
	0.0f,0.0f,1.0f, // 4
	1.0f,0.0f,1.0f, // 5
	1.0f,1.0f,1.0f, // 6
	0.0f,1.0f,1.0f }; // 7
	uint vBuffer = 0;
	uint iBuffer = 0;
	Point3D v[8] = { { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
						{ 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };
};

class Line3D : public Shape3D
{
public:
	Line3D(Point3D vertex0, Point3D vertex1, float lineWidthx = 1) : Shape3D(vertex0, lineWidthx, {0, 0, 0, 0})
	{
		v[0] = vertex0;
		v[1] = vertex1;
	}

	bool Draw()
	{
		glColor3f(255, 255, 255);
		glLineWidth(scale);
		glBegin(GL_LINES);
		glVertex3f(v[0].x, v[0].y, v[0].z);
		glVertex3f(v[1].x, v[1].y, v[1].z);
		glEnd();

		return true;
	}

	Point3D* GetVertexs()
	{
		return v;
	}

	int GetNumVertex()
	{
		return 2;
	}

private:
	Point3D v[2] = {};
};

class Pyramid3D : public Shape3D
{
public:

	Pyramid3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, float h = 1.0f, float s = 1.0f, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot)
	{
		height = h;

		v[4].y *= height;

		for (int i = 0; i < 15; i++)
		{
			float p[3] = { pos.x, pos.y, pos.z };
			vertex[i] *= s;
			vertex[i] += p[i % 3];

			if (i < 5)
			{
				v[i] *= s;
				v[i] += pos;
			}
		}
	}

	bool Draw()
	{
		glColor3f(255, 255, 255);

		glGenBuffers(1, &vBuffer);
		glGenBuffers(1, &iBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);

		glRotatef(rotation.angle, rotation.planeX, rotation.planeY, rotation.planeZ);

		if (axis) DrawAxis();

		return true;
	}

	void DrawDirectMode()
	{
		glColor3f(255, 255, 255);

		glBegin(GL_TRIANGLES);
		// front face =================
		glVertex3f(v[4].x, v[4].y, v[4].z); // v4
		glVertex3f(v[1].x, v[1].y, v[1].z); // v1
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0

		// left face =================
		glVertex3f(v[4].x, v[4].y, v[4].z); // v4
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0
		glVertex3f(v[3].x, v[3].y, v[3].z); // v3

		// right face =================
		glVertex3f(v[4].x, v[4].y, v[4].z); // v4
		glVertex3f(v[2].x, v[2].y, v[2].z); // v2
		glVertex3f(v[1].x, v[1].y, v[1].z); // v1

		// back face =================
		glVertex3f(v[3].x, v[3].y, v[3].z); // v3
		glVertex3f(v[2].x, v[2].y, v[2].z); // v2
		glVertex3f(v[4].x, v[4].y, v[4].z); // v4

		// bottom face =================
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0
		glVertex3f(v[1].x, v[1].y, v[1].z); // v1
		glVertex3f(v[2].x, v[2].y, v[2].z); // v2

		glVertex3f(v[2].x, v[2].y, v[2].z); // v2
		glVertex3f(v[3].x, v[3].y, v[3].z); // v3
		glVertex3f(v[0].x, v[0].y, v[0].z); // v0

		glEnd();

		glRotatef(rotation.angle, rotation.planeX, rotation.planeY, rotation.planeZ);
	}

	void Reset()
	{
		v[0] = { 0.0f, 0.0f, 0.0f };
		v[1] = { 1.0f, 0.0f, 0.0f };
		v[2] = { 1.0f, 1.0f, 0.0f };
		v[3] = { 0.0f, 0.0f, 1.0f };
		v[4] = { 0.5f, 1.0f, 0.5f };
	}

	Point3D* GetVertexs()
	{
		return v;
	}

	int GetNumVertex()
	{
		return 5;
	}

private:
	GLushort index[18] = {
	0, 4, 1,
	1, 4, 2,
	2, 4, 3,
	3, 4, 0,
	3, 0, 2,
	0, 1, 2
	};
	GLfloat vertex[15] = {
	0.0f,0.0f,0.0f, // 0
	1.0f,0.0f,0.0f, // 1
	1.0f,0.0f,1.0f, // 2
	0.0f,0.0f,1.0f, // 3
	0.5f,1.0f,0.5f }; // 4
	uint vBuffer = 0;
	uint iBuffer = 0;

	float height = 1;
	Point3D v[5] = { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, 
					 { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f, 0.5f } };
};

class Cylinder3D : public Shape3D
{
public:
	Cylinder3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, int numSegment = 6, float h = 1.0f, float r = 1.0f, float s = 1.0f, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot)
	{
		segments = numSegment;
		height = h;
		radius = r;
	}

	bool Draw()
	{
		glColor3f(255, 255, 255);

		glBegin(GL_TRIANGLES);

		for (int ii = 0; ii < segments; ii++)
		{
			float theta1 = 2.0f * PI * float(ii) / float(segments); //get the current angle

			int index = ii + 1;
			if (ii == (segments - 1))
			{
				index = 0;
			}

			float theta2 = 2.0f * PI * float(index) / float(segments); //get the current angle

			Point pos1 = { scale * radius * cosf(theta1), scale * radius * sinf(theta1) }; // {x, z}
			Point pos2 = { scale * radius * cosf(theta2), scale * radius * sinf(theta2) }; // {x, z}

			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);
			glVertex3f(0 + position.x, position.y, 0 + position.z);

			glVertex3f(position.x, position.y + (scale * height), position.z);
			glVertex3f(pos2.x + position.x, position.y + (scale * height), pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y + (scale * height), pos1.y + position.z);

			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + (scale * height), pos2.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);

			glVertex3f(pos1.x + position.x, position.y + (scale * height), pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + (scale * height), pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);


		}
		glEnd();

		glRotatef(rotation.angle, rotation.planeX, rotation.planeY, rotation.planeZ);

		if (axis) DrawAxis();

		return true;
	}

private:

	float radius = 1;
	float height = 1;
	int segments = 0;
};

class Plane3D : public Shape3D
{
public:
	Plane3D(Point3D pos = { 0, 0, 0 }, Point3D n = { 0, 1, 0 }, float s = 4, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, (s * 50), rot)
	{
		normal = n;
	}

	bool Draw()
	{
		glColor3f(255, 255, 255);

		glLineWidth(1.0f);

		glBegin(GL_LINES);

		float d = scale;

		for (float i = -d; i <= d; i += 1.0f)
		{
			glVertex3f(i + position.x, position.y, -d + position.z);
			glVertex3f(i + position.x, position.y, d + position.z);
			glVertex3f(-d + position.x, position.y, i + position.z);
			glVertex3f(d + position.x, position.y, i + position.z);
		}

		glEnd();

		if (axis) DrawAxis();

		return true;
	}

private:

	Point3D normal = { 0.0f, 0.0f, 0.0f };
};