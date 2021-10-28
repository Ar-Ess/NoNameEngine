#pragma once
#include "Globals.h"
#include "External/glew/glew.h"
#include "External/SDL/include/SDL_opengl.h"
#include <gl/GL.h>
#include <gl/GLU.h>

#pragma comment (lib, "Source/External/glew/glew32.lib")    /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

class Cube3D
{
public:
	Cube3D(Point3D pos = {0.0f, 0.0f, 0.0f}, float s = 1, Rotation rot = { 0, 0, 0, 0 })
	{
		position = pos;
		scale = s;
		rotation = rot;

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

	void Draw()
	{
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

	Point3D position = { 0.0f, 0.0f, 0.0f };
	float scale = 1;
	Rotation rotation = {0, 0, 0, 0};
	bool selected = false;

	Point3D v[8] = { { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f, 0.0f }, { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f },
						{ 1.0f, 0.0f, 1.0f }, { 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f, 1.0f }, { 0.0f, 0.0f, 1.0f } };
};

class Line3D
{
public:
	Line3D(Point3D vertex0, Point3D vertex1, float lineWidthx = 1)
	{
		v[0] = vertex0;
		v[1] = vertex1;
		lineWidth = lineWidthx;
	}

	void Draw()
	{
		glLineWidth(lineWidth);
		glBegin(GL_LINES);
		glVertex3f(v[0].x, v[0].y, v[0].z);
		glVertex3f(v[1].x, v[1].y, v[1].z);
		glEnd();
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
	float lineWidth = 1;
};

class Pyramid3D
{
public:

	Pyramid3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, float h = 1.0f, float s = 1.0f, Rotation rot = { 0, 0, 0, 0 })
	{
		position = pos;
		scale = s;
		rotation = rot;
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

	void Draw()
	{
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
	}

	void DrawDirectMode()
	{
		if (selected)
		{
			Reset();

			for (int i = 0; i < 5; i++)
			{
				v[i] *= scale;
				v[i] += position;
			}
		}

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

	Point3D position = { 0.0f, 0.0f, 0.0f };
	float scale = 1;
	float height = 1;
	Rotation rotation = { 0, 0, 0, 0 };
	bool selected = false;
	Point3D v[5] = { { 0.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 0.0f, 1.0f }, 
					 { 0.0f, 0.0f, 1.0f }, { 0.5f, 1.0f, 0.5f } };
};

class Sphere3D
{
public:
	Sphere3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, float radius = 1, unsigned int rings = 3, unsigned int sectors = 6)
	{
		float const R = 1. / (float)(rings - 1);
		float const S = 1. / (float)(sectors - 1);

		for (int r = 0; r < rings; ++r) {
			for (int s = 0; s < sectors; ++s) {

				float y = sin(-M_PI_2 + M_PI * r * R);
				float x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
				float z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

				if (x < 0.001f) x = 0.0f;
				if (y < 0.001f) y = 0.0f;
				if (z < 0.001f) z = 0.0f;

				//push_back(vec2(s * S, r * R));
				vertex.push_back(x * radius);
				vertex.push_back(y * radius);
				vertex.push_back(z * radius);

				if (r < rings - 1) push_indices(sectors, r, s);
			}
		}

		for (int i = 0; i < (rings * sectors * 3); i++)
		{
			if (pos == 0) break;
			float p[3] = { pos.x, pos.y, pos.z };
			vertex[i] += p[i % 3];
		}
	}

	void Draw()
	{
		float a[12] = {};
		float i[12] = {};
		std::copy(vertex.begin(), vertex.end(), a);
		std::copy(index.begin(), index.end(), i);

		glGenBuffers(1, &vBuffer);
		glGenBuffers(1, &iBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex.size(), &a, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * index.size(), &i, GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, vertex.size(), GL_UNSIGNED_SHORT, 0);

		//glEnableClientState(GL_VERTEX_ARRAY);
		////glEnableClientState(GL_NORMAL_ARRAY);
		////glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		//glVertexPointer(3, GL_FLOAT, 0, &vertex[0]);
		////glNormalPointer(GL_FLOAT, 0, &normals[0]);
		////glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
		//glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_SHORT, &index[0]);

		//glDisableClientState(GL_VERTEX_ARRAY);
	}

private:
	void push_indices(int sectors, int r, int s) 
	{

		int curRow = r * sectors;
		int nextRow = (r + 1) * sectors;
		int nextS = (s + 1) % sectors;

		index.push_back(curRow + s);
		index.push_back(nextRow + s);
		index.push_back(nextRow + nextS);

		index.push_back(curRow + s);
		index.push_back(nextRow + nextS);
		index.push_back(curRow + nextS);
	}

private:

	std::vector<GLfloat> vertex;
	std::vector<GLfloat> normals;
	std::vector<vec2> texcoords;
	std::vector<GLushort> index;

	Point3D position = {0.0f, 0.0f, 0.0f};

	uint vBuffer = 0;
	uint iBuffer = 0;
};

class Cylinder3D
{
public:
	Cylinder3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, int numSegment = 6, float h = 1.0f, float r = 1.0f, Rotation rot = {0, 0, 0, 0})
	{
		position = pos;
		segments = numSegment;
		height = h;
		radius = r;
	}

	void DrawDirectMode()
	{
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

			Point pos1 = { radius * cosf(theta1), radius * sinf(theta1) }; // {x, z}
			Point pos2 = { radius * cosf(theta2), radius * sinf(theta2) }; // {x, z}
			
			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);
			glVertex3f(0 + position.x, position.y, 0 + position.z);

			glVertex3f(position.x, position.y + height, position.z);
			glVertex3f(pos2.x + position.x, position.y + height, pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y + height, pos1.y + position.z);

			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + height, pos2.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);

			glVertex3f(pos1.x + position.x, position.y + height, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + height, pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);


		}
		glEnd();

		glRotatef(rotation.angle, rotation.planeX, rotation.planeY,rotation.planeZ);
	}

private:

	Point3D position = { 0.0f, 0.0f, 0.0f };
	float radius = 1;
	float height = 1;
	int segments = 0;
	Rotation rotation = { 0, 0, 0, 0 };
	bool selected = false;
};

