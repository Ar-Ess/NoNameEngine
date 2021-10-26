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

	Point3D* GetVertexs()
	{
		return v;
	}

private:
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

		for (int i = 0; i < 5; i++)
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

private:
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
	Sphere3D(Point3D pos = {0, 0, 0}, float radius = 1, unsigned int rings = 2, unsigned int sectors = 4)
	{
		float const R = 1. / (float)(rings - 1);
		float const S = 1. / (float)(sectors - 1);
		int r, s;

		vertices.resize(rings * sectors * 3);
		normals.resize(rings * sectors * 3);
		texcoords.resize(rings * sectors * 2);
		std::vector<GLfloat>::iterator v = vertices.begin();
		std::vector<GLfloat>::iterator n = normals.begin();
		std::vector<GLfloat>::iterator t = texcoords.begin();
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			float const y = sin(-M_PI_2 + M_PI * r * R);
			float const x = cos(2 * M_PI * s * S) * sin(M_PI * r * R);
			float const z = sin(2 * M_PI * s * S) * sin(M_PI * r * R);

			*t++ = s * S;
			*t++ = r * R;

			*v++ = x * radius;
			*v++ = y * radius;
			*v++ = z * radius;

			*n++ = x;
			*n++ = y;
			*n++ = z;
		}

		indices.resize(rings * sectors * 4);
		std::vector<GLushort>::iterator i = indices.begin();
		for (r = 0; r < rings; r++) for (s = 0; s < sectors; s++) {
			*i++ = r * sectors + s;
			*i++ = r * sectors + (s + 1);
			*i++ = (r + 1) * sectors + (s + 1);
			*i++ = (r + 1) * sectors + s;
		}
	}

	void Draw()
	{
		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glTranslatef(position.x, position.y, position.z);

		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, 0, &vertices[0]);
		glNormalPointer(GL_FLOAT, 0, &normals[0]);
		glTexCoordPointer(2, GL_FLOAT, 0, &texcoords[0]);
		glDrawElements(GL_QUADS, indices.size(), GL_UNSIGNED_SHORT, &indices[0]);
		glPopMatrix();
	}

private:
	std::vector<GLfloat> vertices;
	std::vector<GLfloat> normals;
	std::vector<GLfloat> texcoords;
	std::vector<GLushort> indices;
	Point3D position = {0, 0, 0};
};
