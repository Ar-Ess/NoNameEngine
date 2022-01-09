#pragma once
#include "Globals.h"
#include <string>
#include <iostream>

#include "External/glew/glew.h"
#include "External/SDL/include/SDL_opengl.h"
#include "External/MathGeoLib/Geometry/AABB.h"
#include "External/MathGeoLib/Geometry/OBB.h"
#include <gl/GL.h>

#include "Component.h"

#pragma comment (lib, "Source/External/glew/glew32.lib") /* link OpenGL Utility lib     */
#pragma comment (lib, "opengl32.lib") /* link Microsoft OpenGL lib   */

struct Point3D;
using namespace std;

enum ShapeType
{
	EMPTY3D,
	CUBE3D,
	LINE3D,
	PYRAMID3D,
	CYLINDER3D,
	PLANE3D,
	SPHERE3D,
	MODEL3D,
};

class Shape3D
{
public:

	virtual ~Shape3D() {}

	virtual bool Draw() { return true; }

	Point3D GetPosition() const
	{
		return position;
	}

	Rotation GetRotation() const
	{
		return rotation;
	}

	Point3D GetScale() const
	{
		return scale;
	}

	ShapeType GetShapeType()
	{
		return type;
	}

	string WriteShapeType()
	{
		switch (type)
		{
		case CUBE3D: return string("Cube 3D"); break;
		case LINE3D: return string("Line 3D"); break;
		case PYRAMID3D: return string("Pyramid 3D"); break;
		case CYLINDER3D: return string("Cylinder 3D"); break;
		case PLANE3D: return string("Plane 3D"); break;
		case SPHERE3D: return string("Sphere 3D"); break;
		case MODEL3D: return string("Model 3D"); break;
		case EMPTY3D: return string("Empty 3D"); break;
		}

		return string("NULL");
	}

	ShapeType ReadShapeType(const char* type)
	{
		if (SameString(type, "Cube 3D")) return CUBE3D;
		if (SameString(type, "Line 3D")) return LINE3D;
		if (SameString(type, "Pyramid 3D")) return PYRAMID3D;
		if (SameString(type, "Cylinder 3D")) return CYLINDER3D;
		if (SameString(type, "Plane 3D")) return PLANE3D;
		if (SameString(type, "Sphere 3D")) return SPHERE3D;
		if (SameString(type, "Model 3D")) return MODEL3D;

		return EMPTY3D;
	}

	const char* GetName() const
	{
		return name.c_str();
	}

	void SetName(const char* nameSet)
	{
		name.clear();
		name += nameSet;
	}

	void SetPosition(Point3D position)
	{
		this->position = position;
	}

	void SetScale(Point3D scale)
	{
		this->scale = scale;
	}

	void SetRotation(Rotation rotation)
	{
		this->rotation = rotation;
	}

	void DeleteChilds(vector<Shape3D*>* vect)
	{
		for (unsigned int i = 0; i < childs.size(); i++)
		{
			if (!vect->at(i)->childs.empty())
			{
				DeleteChilds(&vect->at(i)->childs);
			}

			vect->at(i)->~Shape3D();
			delete vect->at(i);
			vect->at(i) = nullptr;
		}

		vect->clear();
	}

	bool axis = false;
	bool edges = true;
	bool solid = true;
	bool normals = false;
	bool selected = false;
	bool draw = true;
	bool hasParent = false;

	vector<Shape3D*> childs;
	Shape3D* parent = nullptr;
	int id = 0;
	AABB boundingBox;
	vector<Component*> components;

protected:

	Shape3D(Point3D pos, Point3D s, Rotation rot, ShapeType sT, const char* n, bool ax = false)
	{
		position = pos;
		rotation = rot;
		scale = s;
		axis = ax;
		selected = false;
		type = sT;
		name.clear();
		name += n;
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
	Point3D scale = { 1, 1, 1 };
	ShapeType type = EMPTY3D;
	string name;
};

class Empty3D : public Shape3D
{
public:
	Empty3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, Point3D s = { 1, 1, 1 }, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot, EMPTY3D, "Empty", true)
	{
	}

	~Empty3D()
	{
		DeleteChilds(&childs);
	}

	bool Draw()
	{
		if (!draw) return true;
		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);
		glScalef(scale.x, scale.y, scale.z);
		glLineWidth(1.0f);

		if (axis) DrawAxis();

		for (unsigned int i = 0; !childs.empty() && i < childs.size(); i++) childs[i]->Draw();

		return true;
	}

private:

};

class Cube3D : public Shape3D
{
public:
	Cube3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, Point3D s = { 1, 1, 1 } , Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot, CUBE3D, "Cube")
	{
		//for (int i = 0; i < 24; i++)
		//{
		//	float p[3] = {pos.x, pos.y, pos.z};
		//	vertex[i] *= s;
		//	vertex[i] += p[i % 3];

		//	if (i < 8)
		//	{
		//		v[i] *= s;
		//		v[i] += pos;
		//	}
		//}
	}

	~Cube3D()
	{
		DeleteChilds(&childs);
	}

	AABB cubeBox;

	bool Draw()
	{
		if (!draw) return true;
		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);
		glScalef(scale.x, scale.y, scale.z);
		glLineWidth(1.0f);

		if (solid) DrawSolid();
		if (edges) DrawEdges();
		if (axis) DrawAxis();

		for (unsigned int i = 0; !childs.empty() && i < childs.size(); i++) childs[i]->Draw();

		return true;

		// Buffer Mode
		/*glColor3f(255, 255, 255);

		glGenBuffers(1, &vBuffer);
		glGenBuffers(1, &iBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);*/
	}

private:

	void DrawSolid()
	{
		glColor3f(255, 255, 255);
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
	}
	bool DrawEdges()
	{
		glColor3f(255, 0, 0);
		glBegin(GL_LINES);
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

		return true;
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
	Line3D(Point3D vertex0 = { 0, 0, 0 }, Point3D vertex1 = { 0, 5, 0 }, float lineWidthx = 1) : Shape3D(vertex0, { lineWidthx, lineWidthx, lineWidthx }, { 0, 0, 0, 0 }, LINE3D, "Line")
	{
		v[0] = vertex0;
		v[1] = vertex1;
	}

	~Line3D()
	{
		DeleteChilds(&childs);
	}

	bool Draw()
	{
		if (!draw) return true;

		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);

		if (solid) DrawSolid();
		if (axis) DrawAxis();

		for (unsigned int i = 0; !childs.empty() && i < childs.size(); i++) childs[i]->Draw();

		return true;
	}

	Point3D* GetVertexs()
	{
		return v;
	}

	Point3D GetSecondVertex()
	{
		return v[1];
	}

	int GetNumVertex()
	{
		return 2;
	}

private:

	bool DrawSolid()
	{
		glColor3f(255, 255, 255);
		glLineWidth(scale.x);
		glBegin(GL_LINES);
		glVertex3f(v[0].x, v[0].y, v[0].z);
		glVertex3f(v[1].x, v[1].y, v[1].z);
		glEnd();

		return true;
	}

private:
	Point3D v[2] = {};
};

class Pyramid3D : public Shape3D
{
public:

	Pyramid3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, float h = 1.0f, Point3D s = { 1, 1, 1 }, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot, PYRAMID3D, "Pyramid")
	{
		height = h;

		v[4].y *= height;

		/*for (int i = 0; i < 15; i++)
		{
			float p[3] = { pos.x, pos.y, pos.z };
			vertex[i] *= s;
			vertex[i] += p[i % 3];

			if (i < 5)
			{
				v[i] *= s;
				v[i] += pos;
			}
		}*/
	}

	~Pyramid3D()
	{
		DeleteChilds(&childs);
	}

	bool Draw()
	{
		if (!draw) return true;

		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);
		glScalef(scale.x, scale.y, scale.z);
		glLineWidth(1.0f);

		if (solid) DrawSolid();
		if (edges) DrawEdges();
		if (axis) DrawAxis();


		for (unsigned int i = 0; !childs.empty() && i < childs.size(); i++) childs[i]->Draw();
		
		return true;

		// Buffer Mode
		/*glColor3f(255, 255, 255);

		glGenBuffers(1, &vBuffer);
		glGenBuffers(1, &iBuffer);

		glBindBuffer(GL_ARRAY_BUFFER, vBuffer);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iBuffer);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_STATIC_DRAW);

		glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_SHORT, 0);*/
	}

private:

	void DrawSolid()
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
	}
	bool DrawEdges()
	{
		glColor3f(255, 0, 0);

		glBegin(GL_LINES);
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

		return true;
	}

	void Reset()
	{
		v[0] = { 0.0f, 0.0f, 0.0f };
		v[1] = { 1.0f, 0.0f, 0.0f };
		v[2] = { 1.0f, 1.0f, 0.0f };
		v[3] = { 0.0f, 0.0f, 1.0f };
		v[4] = { 0.5f, 1.0f, 0.5f };
	}

public: // Getters

	Point3D* GetVertexs()
	{
		return v;
	}

	int GetNumVertex() const 
	{
		return 5;
	}

	float GetHeight() const
	{
		return height;
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
	Cylinder3D(Point3D pos = { 0.0f, 0.0f, 0.0f }, int numSegment = 18, float h = 4.0f, float r = 1.0f, Point3D s = { 1, 1, 1 }, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot, CYLINDER3D, "Cylinder")
	{
		segments = numSegment;
		height = h;
		radius = r;
	}

	~Cylinder3D()
	{
		DeleteChilds(&childs);
	}

	bool Draw()
	{
		if (!draw) return true;

		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);
		glScalef(scale.x, scale.y, scale.z);
		glLineWidth(1.0f);

		if (solid) DrawSolid();
		if (edges) DrawEdges();
		if (axis) DrawAxis();

		for (unsigned int i = 0; !childs.empty() && i < childs.size(); i++) childs[i]->Draw();

		return true;
	}

private:

	bool DrawSolid()
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

			Point pos1 = { radius * cosf(theta1), radius * sinf(theta1) }; // {x, z}
			Point pos2 = { radius * cosf(theta2), radius * sinf(theta2) }; // {x, z}

			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);
			glVertex3f(0 + position.x, position.y, 0 + position.z);

			glVertex3f(position.x, position.y + (height), position.z);
			glVertex3f(pos2.x + position.x, position.y + (height), pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y + (height), pos1.y + position.z);

			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + (height), pos2.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);

			glVertex3f(pos1.x + position.x, position.y + (height), pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + (height), pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);


		}

		glEnd();

		return true;
	}
	bool DrawEdges()
	{
		glColor3f(255, 0, 0);

		glBegin(GL_LINES);

		for (int ii = 0; ii < segments; ii++)
		{
			float theta1 = 2.0f * PI * float(ii) / float(segments); //get the current angle

			int index = ii + 1;
			if (ii == (segments - 1))
			{
				index = 0;
			}

			float theta2 = 2.0f * PI * float(index) / float(segments); //get the current angle

			Point pos1 = {radius * cosf(theta1), radius * sinf(theta1) }; // {x, z}
			Point pos2 = {radius * cosf(theta2), radius * sinf(theta2) }; // {x, z}

			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);
			glVertex3f(0 + position.x, position.y, 0 + position.z);

			glVertex3f(position.x, position.y + (height), position.z);
			glVertex3f(pos2.x + position.x, position.y + (height), pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y + (height), pos1.y + position.z);

			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + (height), pos2.y + position.z);
			glVertex3f(pos2.x + position.x, position.y, pos2.y + position.z);

			glVertex3f(pos1.x + position.x, position.y + (height), pos1.y + position.z);
			glVertex3f(pos2.x + position.x, position.y + (height), pos2.y + position.z);
			glVertex3f(pos1.x + position.x, position.y, pos1.y + position.z);


		}
		glEnd();


		return true;
	}

public: // Getters

	float GetRadius() const
	{
		return radius;
	}

	float GetHeight() const
	{
		return height;
	}

	int GetSegments() const
	{
		return segments;
	}

private:

	float radius = 1;
	float height = 1;
	int segments = 0;
};

class Plane3D : public Shape3D
{
public:
	Plane3D(Point3D pos = { 0, 0, 0 }, Point3D n = { 0, 1, 0 }, Point3D s = { 1,1,1 }, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot, PLANE3D, "Plane")
	{
		if ((bool)n.x)
			n.Set(1, 0, 0);

		else if ((bool)n.y)
			n.Set(0, 1, 0);

		else
			n.Set(0, 0, 1);

		normal = n;
	}

	~Plane3D()
	{
		DeleteChilds(&childs);
	}

	bool Draw()
	{
		if (!draw) return true;

		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);
		glLineWidth(1.0f);

		if (solid) DrawSolid();
		if (edges) DrawEdges();
		if (axis) DrawAxis();

		for (unsigned int i = 0; !childs.empty() && i < childs.size(); i++) childs[i]->Draw();

		return true;
	}

private:

	bool DrawSolid()
	{
		glColor3f(255, 255, 255);

		glLineWidth(1.0f);

		glBegin(GL_QUADS);

		float d = 0;
		float p = 0;

		if ((bool)normal.x)
		{
			p = scale.y;
			d = scale.z;

			glVertex3f(position.x, position.y - p, position.z - d);
			glVertex3f(position.x, position.y - p, position.z + d);
			glVertex3f(position.x, position.y + p, position.z + d);
			glVertex3f(position.x, position.y + p, position.z - d);
		}
		else if ((bool)normal.y)
		{
			p = scale.x;
			d = scale.z;
			glVertex3f(position.x - p, position.y, position.z - d);
			glVertex3f(position.x - p, position.y, position.z + d);
			glVertex3f(position.x + p, position.y, position.z + d);
			glVertex3f(position.x + p, position.y, position.z - d);
		}
		else if ((bool)normal.z)
		{
			p = scale.x;
			d = scale.y;
			glVertex3f(position.x - p, position.y - d, position.z);
			glVertex3f(position.x - p, position.y + d, position.z);
			glVertex3f(position.x + p, position.y + d, position.z);
			glVertex3f(position.x + p, position.y - d, position.z);
		}

		glEnd();

		return true;
	}
	bool DrawEdges()
	{
		glColor3f(255, 255, 255);

		glLineWidth(1.0f);

		glBegin(GL_LINES);

		float d = 0;
		float p = 0;

		if ((bool)normal.x)
		{
			p = scale.y;
			d = scale.z;
			for (float i = -d; i <= d; i += 1.0f)
			{

				glVertex3f(     position.x,  i + position.y, -d + position.z);
				glVertex3f(     position.x,  i + position.y,  d + position.z);
				glVertex3f(     position.x, -p + position.y,  i + position.z);
				glVertex3f(     position.x,  p + position.y,  i + position.z);
			}
		}
		else if ((bool)normal.y)
		{
			p = scale.x;
			d = scale.z;
			for (float i = -d; i <= d; i += 1.0f)
			{

				glVertex3f( i + position.x,      position.y, -d + position.z);
				glVertex3f( i + position.x,      position.y,  d + position.z);
				glVertex3f(-p + position.x,      position.y,  i + position.z);
				glVertex3f( p + position.x,      position.y,  i + position.z);
			}
		}
		else
		{
			p = scale.x;
			d = scale.y;
			for (float i = -d; i <= d; i += 1.0f)
			{

				glVertex3f( i + position.x, -d + position.y,      position.z);
				glVertex3f( i + position.x,  d + position.y,      position.z);
				glVertex3f(-p + position.x,  i + position.y,      position.z);
				glVertex3f( p + position.x,  i + position.y,      position.z);
			}
		}

		glEnd();

		return true;
	}

public: // Getters

	Point3D GetNormal() const
	{
		return normal;
	}

private:

	Point3D normal = { 0.0f, 0.0f, 0.0f };
};

class Sphere3D : public Shape3D
{
public:
	Sphere3D(Point3D pos = { 0, 0, 0 }, Point3D s = { 1, 1, 1 }, float r = 1.0f, int subd = 1, Rotation rot = { 0, 0, 0, 0 }) : Shape3D(pos, s, rot, SPHERE3D, "Sphere")
	{
		radius = r;
		subdivision = subd;
		interleavedStride = 32; // Do not change!

		bool smooth = false;

		if (smooth)
			BuildVerticesSmooth();
		else
			BuildVerticesFlat();
	}

	~Sphere3D()
	{
		DeleteChilds(&childs);
	}

	bool Draw()
	{
		if (!draw) return true;

		glTranslatef(position.x, position.y, position.z);
		glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);
		glScalef(scale.x, scale.y, scale.z);
		glLineWidth(1.0f);

		if (solid) DrawSolid();
		if (edges) DrawEdges();
		if (axis) DrawAxis();

		for (unsigned int i = 0; !childs.empty() && i < childs.size(); i++) childs[i]->Draw();

		return true;
	}

private:

	bool DrawSolid()
	{
		glColor3f(255, 255, 255);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(1.0, 1.0f);
		// interleaved array
		glEnableClientState(GL_VERTEX_ARRAY);
		glEnableClientState(GL_NORMAL_ARRAY);
		//glEnableClientState(GL_TEXTURE_COORD_ARRAY);

		glVertexPointer(3, GL_FLOAT, interleavedStride, &interleavedVertices[0]);
		glNormalPointer(GL_FLOAT, interleavedStride, &interleavedVertices[3]);
		//glTexCoordPointer(2, GL_FLOAT, interleavedStride, &interleavedVertices[6]);

		glDrawElements(GL_TRIANGLES, (unsigned int)index.size(), GL_UNSIGNED_INT, index.data());

		glDisableClientState(GL_VERTEX_ARRAY);
		glDisableClientState(GL_NORMAL_ARRAY);
		//glDisableClientState(GL_TEXTURE_COORD_ARRAY);

		glDisable(GL_POLYGON_OFFSET_FILL);

		return true;
	}
	bool DrawEdges()
	{
		// set line colour
		const float color[4] = {255.0f, 0.0f, 0.0f, 255.0f};
		glColor4fv(color);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, color);

		// draw lines with VA
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glEnableClientState(GL_VERTEX_ARRAY);
		glVertexPointer(3, GL_FLOAT, 0, vertex.data());

		glDrawElements(GL_LINES, (unsigned int)lineIndex.size(), GL_UNSIGNED_INT, lineIndex.data());

		glDisableClientState(GL_VERTEX_ARRAY);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);

		return true;
	}

public: // Getters & Setters

	float GetRadius() const { return radius; }
	void SetRadius(float radius)
	{
		this->radius = radius;
		UpdateRadius(); // update vertex positions only
	}
	int GetSubdivision() const { return subdivision; }
	void SetSubdivision(int iteration)
	{
		this->subdivision = iteration;
		BuildVerticesFlat();
	}

	unsigned int GetVertexCount() const { return (unsigned int)vertex.size() / 3; }
	unsigned int GetNormalCount() const { return (unsigned int)normals.size() / 3; }
	unsigned int GetTexCoordCount() const { return (unsigned int)texCoords.size() / 2; }
	unsigned int GetIndexCount() const { return (unsigned int)index.size(); }
	unsigned int GetLineIndexCount() const { return (unsigned int)lineIndex.size(); }
	unsigned int GetTriangleCount() const { return GetIndexCount() / 3; }

	unsigned int GetVertexSize() const { return (unsigned int)vertex.size() * sizeof(float); }   // # of bytes
	unsigned int GetNormalSize() const { return (unsigned int)normals.size() * sizeof(float); }
	unsigned int GetTexCoordSize() const { return (unsigned int)texCoords.size() * sizeof(float); }
	unsigned int GetIndexSize() const { return (unsigned int)index.size() * sizeof(unsigned int); }
	unsigned int GetLineIndexSize() const { return (unsigned int)lineIndex.size() * sizeof(unsigned int); }

	const float* GetVertices() const { return vertex.data(); }
	const float* GetNormals() const { return normals.data(); }
	const float* GetTexCoords() const { return texCoords.data(); }
	const unsigned int* GetIndex() const { return index.data(); }
	const unsigned int* GetLineIndex() const { return lineIndex.data(); }

	// for interleaved vertices: V/N/T
	unsigned int GetInterleavedVertexCount() const { return GetVertexCount(); }    // # of vertices
	unsigned int GetInterleavedVertexSize() const { return (unsigned int)interleavedVertices.size() * sizeof(float); }    // # of bytes
	int GetInterleavedStride() const { return interleavedStride; }   // should be 32 bytes
	const float* GetInterleavedVertices() const { return interleavedVertices.data(); }

	void DebugPring() const
	{

		std::cout << "===== Icosphere =====\n"
			<< "        Radius: " << radius << "\n"
			<< "   Subdivision: " << subdivision << "\n"
			//<< "    Smoothness: " << (smooth ? "true" : "false") << "\n"
			<< "Triangle Count: " << GetTriangleCount() << "\n"
			<< "   Index Count: " << GetIndexCount() << "\n"
			<< "  Vertex Count: " << GetVertexCount() << "\n"
			<< "  Normal Count: " << GetNormalCount() << "\n"
			<< "TexCoord Count: " << GetTexCoordCount() << std::endl;
	}

private:

	// static functions
	void ComputeFaceNormal(const float v1[3], const float v2[3], const float v3[3], float n[3])
	{
		const float EPSILON = 0.000001f;

		// default return value (0, 0, 0)
		n[0] = n[1] = n[2] = 0;

		// find 2 edge vectors: v1-v2, v1-v3
		float ex1 = v2[0] - v1[0];
		float ey1 = v2[1] - v1[1];
		float ez1 = v2[2] - v1[2];
		float ex2 = v3[0] - v1[0];
		float ey2 = v3[1] - v1[1];
		float ez2 = v3[2] - v1[2];

		// cross product: e1 x e2
		float nx, ny, nz;
		nx = ey1 * ez2 - ez1 * ey2;
		ny = ez1 * ex2 - ex1 * ez2;
		nz = ex1 * ey2 - ey1 * ex2;

		// normalize only if the length is > 0
		float length = sqrtf(nx * nx + ny * ny + nz * nz);
		if (length > EPSILON)
		{
			// normalize
			float lengthInv = 1.0f / length;
			n[0] = nx * lengthInv;
			n[1] = ny * lengthInv;
			n[2] = nz * lengthInv;
		}
	}
	void ComputeVertexNormal(const float v[3], float normal[3])
	{
		// normalize
		float scale = ComputeScaleForLength(v, 1);
		normal[0] = v[0] * scale;
		normal[1] = v[1] * scale;
		normal[2] = v[2] * scale;
	}
	float ComputeScaleForLength(const float v[3], float length)
	{
		// and normalize the vector then re-scale to new radius
		return length / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	}
	void ComputeHalfVertex(const float v1[3], const float v2[3], float length, float newV[3])
	{
		newV[0] = v1[0] + v2[0];
		newV[1] = v1[1] + v2[1];
		newV[2] = v1[2] + v2[2];
		float scale = ComputeScaleForLength(newV, length);
		newV[0] *= scale;
		newV[1] *= scale;
		newV[2] *= scale;
	}
	void ComputeHalfTexCoord(const float t1[2], const float t2[2], float newT[2])
	{
		newT[0] = (t1[0] + t2[0]) * 0.5f;
		newT[1] = (t1[1] + t2[1]) * 0.5f;
	}
	bool IsSharedTexCoord(const float t[2])
	{
		// 20 non-shared line segments
		const float S = 1.0f / 11;  // texture steps
		const float T = 1.0f / 3;
		static float segments[] = { S, 0,       0, T,       // 00 - 05
									S, 0,       S * 2, T,     // 00 - 06
									S * 3, 0,     S * 2, T,     // 01 - 06
									S * 3, 0,     S * 4, T,     // 01 - 07
									S * 5, 0,     S * 4, T,     // 02 - 07
									S * 5, 0,     S * 6, T,     // 02 - 08
									S * 7, 0,     S * 6, T,     // 03 - 08
									S * 7, 0,     S * 8, T,     // 03 - 09
									S * 9, 0,     S * 8, T,     // 04 - 09
									S * 9, 0,     1, T * 2,     // 04 - 14
									0, T,       S * 2, 1,     // 05 - 15
									S * 3, T * 2,   S * 2, 1,     // 10 - 15
									S * 3, T * 2,   S * 4, 1,     // 10 - 16
									S * 5, T * 2,   S * 4, 1,     // 11 - 16
									S * 5, T * 2,   S * 6, 1,     // 11 - 17
									S * 7, T * 2,   S * 6, 1,     // 12 - 17
									S * 7, T * 2,   S * 8, 1,     // 12 - 18
									S * 9, T * 2,   S * 8, 1,     // 13 - 18
									S * 9, T * 2,   S * 10, 1,    // 13 - 19
									1, T * 2,     S * 10, 1 };  // 14 - 19

		// check the point with all 20 line segments
		// if it is on the line segment, it is non-shared
		int count = (int)(sizeof(segments) / sizeof(segments[0]));
		for (int i = 0, j = 2; i < count; i += 4, j += 4)
		{
			if (IsOnLineSegment(&segments[i], &segments[j], t))
				return false;   // not shared
		}

		return true;
	}

	bool IsOnLineSegment(const float a[2], const float b[2], const float c[2])
	{
		const float EPSILON = 0.0001f;

		// cross product must be 0 if c is on the line
		float cross = ((b[0] - a[0]) * (c[1] - a[1])) - ((b[1] - a[1]) * (c[0] - a[0]));
		if (cross > EPSILON || cross < -EPSILON)
			return false;

		// c must be within a-b
		if ((c[0] > a[0] && c[0] > b[0]) || (c[0] < a[0] && c[0] < b[0]))
			return false;
		if ((c[1] > a[1] && c[1] > b[1]) || (c[1] < a[1] && c[1] < b[1]))
			return false;

		return true;    // all passed, it is on the line segment
	}

	// member functions
	void UpdateRadius()
	{
		float scale = ComputeScaleForLength(&vertex[0], radius);

		std::size_t i, j;
		std::size_t count = vertex.size();
		for (i = 0, j = 0; i < count; i += 3, j += 8)
		{
			vertex[i] *= scale;
			vertex[i + 1] *= scale;
			vertex[i + 2] *= scale;

			// for interleaved array
			interleavedVertices[j] *= scale;
			interleavedVertices[j + 1] *= scale;
			interleavedVertices[j + 2] *= scale;
		}
	}
	std::vector<float> ComputeIcosahedronVertices()
	{
		const float PII = acos(-1);
		const float H_ANGLE = PII / 180 * 72;    // 72 degree = 360 / 5
		const float V_ANGLE = atanf(1.0f / 2);  // elevation = 26.565 degree

		std::vector<float> vertices(12 * 3);    // 12 vertices
		int i1, i2;                             // indices
		float z, xy;                            // coords
		float hAngle1 = -PII / 2 - H_ANGLE / 2;  // start from -126 deg at 2nd row
		float hAngle2 = -PII / 2;                // start from -90 deg at 3rd row

		// the first top vertex (0, 0, r)
		vertices[0] = 0;
		vertices[1] = 0;
		vertices[2] = radius;

		// 10 vertices at 2nd and 3rd rows
		for (int i = 1; i <= 5; ++i)
		{
			i1 = i * 3;         // for 2nd row
			i2 = (i + 5) * 3;   // for 3rd row

			z = radius * sinf(V_ANGLE);             // elevaton
			xy = radius * cosf(V_ANGLE);

			vertices[i1] = xy * cosf(hAngle1);      // x
			vertices[i2] = xy * cosf(hAngle2);
			vertices[i1 + 1] = xy * sinf(hAngle1);  // x
			vertices[i2 + 1] = xy * sinf(hAngle2);
			vertices[i1 + 2] = z;                   // z
			vertices[i2 + 2] = -z;

			// next horizontal angles
			hAngle1 += H_ANGLE;
			hAngle2 += H_ANGLE;
		}

		// the last bottom vertex (0, 0, -r)
		i1 = 11 * 3;
		vertices[i1] = 0;
		vertices[i1 + 1] = 0;
		vertices[i1 + 2] = -radius;

		return vertices;
	}
	void BuildVerticesFlat()
	{
		//const float S_STEP = 1 / 11.0f;         // horizontal texture step
		//const float T_STEP = 1 / 3.0f;          // vertical texture step
		const float S_STEP = 186 / 2048.0f;     // horizontal texture step
		const float T_STEP = 322 / 1024.0f;     // vertical texture step

		// compute 12 vertices of icosahedron
		std::vector<float> tmpVertices = ComputeIcosahedronVertices();

		// clear memory of prev arrays
		std::vector<float>().swap(vertex);
		std::vector<float>().swap(normals);
		std::vector<float>().swap(texCoords);
		std::vector<unsigned int>().swap(index);
		std::vector<unsigned int>().swap(lineIndex);

		const float* v0, * v1, * v2, * v3, * v4, * v11;          // vertex positions
		float n[3];                                         // face normal
		float t0[2], t1[2], t2[2], t3[2], t4[2], t11[2];    // texCoords
		unsigned int index = 0;

		// compute and add 20 tiangles of icosahedron first
		v0 = &tmpVertices[0];       // 1st vertex
		v11 = &tmpVertices[11 * 3]; // 12th vertex
		for (int i = 1; i <= 5; ++i)
		{
			// 4 vertices in the 2nd row
			v1 = &tmpVertices[i * 3];
			if (i < 5)
				v2 = &tmpVertices[(i + 1) * 3];
			else
				v2 = &tmpVertices[3];

			v3 = &tmpVertices[(i + 5) * 3];
			if ((i + 5) < 10)
				v4 = &tmpVertices[(i + 6) * 3];
			else
				v4 = &tmpVertices[6 * 3];

			// texture coords
			t0[0] = (2 * i - 1) * S_STEP;   t0[1] = 0;
			t1[0] = (2 * i - 2) * S_STEP;   t1[1] = T_STEP;
			t2[0] = (2 * i - 0) * S_STEP;   t2[1] = T_STEP;
			t3[0] = (2 * i - 1) * S_STEP;   t3[1] = T_STEP * 2;
			t4[0] = (2 * i + 1) * S_STEP;   t4[1] = T_STEP * 2;
			t11[0] = 2 * i * S_STEP;         t11[1] = T_STEP * 3;

			// add a triangle in 1st row
			ComputeFaceNormal(v0, v1, v2, n);
			AddVertices(v0, v1, v2);
			AddNormals(n, n, n);
			AddTexCoords(t0, t1, t2);
			AddIndex(index, index + 1, index + 2);

			// add 2 triangles in 2nd row
			ComputeFaceNormal(v1, v3, v2, n);
			AddVertices(v1, v3, v2);
			AddNormals(n, n, n);
			AddTexCoords(t1, t3, t2);
			AddIndex(index + 3, index + 4, index + 5);

			ComputeFaceNormal(v2, v3, v4, n);
			AddVertices(v2, v3, v4);
			AddNormals(n, n, n);
			AddTexCoords(t2, t3, t4);
			AddIndex(index + 6, index + 7, index + 8);

			// add a triangle in 3rd row
			ComputeFaceNormal(v3, v11, v4, n);
			AddVertices(v3, v11, v4);
			AddNormals(n, n, n);
			AddTexCoords(t3, t11, t4);
			AddIndex(index + 9, index + 10, index + 11);

			// add 6 edge lines per iteration
			//  i
			//  /   /   /   /   /       : (i, i+1)                              //
			// /__ /__ /__ /__ /__                                              //
			// \  /\  /\  /\  /\  /     : (i+3, i+4), (i+3, i+5), (i+4, i+5)    //
			//  \/__\/__\/__\/__\/__                                            //
			//   \   \   \   \   \      : (i+9,i+10), (i+9, i+11)               //
			//    \   \   \   \   \                                             //
			lineIndex.push_back(index);       // (i, i+1)
			lineIndex.push_back(index + 1);       // (i, i+1)
			lineIndex.push_back(index + 3);     // (i+3, i+4)
			lineIndex.push_back(index + 4);
			lineIndex.push_back(index + 3);     // (i+3, i+5)
			lineIndex.push_back(index + 5);
			lineIndex.push_back(index + 4);     // (i+4, i+5)
			lineIndex.push_back(index + 5);
			lineIndex.push_back(index + 9);     // (i+9, i+10)
			lineIndex.push_back(index + 10);
			lineIndex.push_back(index + 9);     // (i+9, i+11)
			lineIndex.push_back(index + 11);

			// next index
			index += 12;
		}

		// subdivide icosahedron
		SubdivideVerticesFlat();

		// generate interleaved vertex array as well
		BuildInterleavedVertices();
	}
	void BuildVerticesSmooth()
	{
		//const float S_STEP = 1 / 11.0f;         // horizontal texture step
		//const float T_STEP = 1 / 3.0f;          // vertical texture step
		const float S_STEP = 186 / 2048.0f;     // horizontal texture step
		const float T_STEP = 322 / 1024.0f;     // vertical texture step

		// compute 12 vertices of icosahedron
		// NOTE: v0 (top), v11(bottom), v1, v6(first vert on each row) cannot be
		// shared for smooth shading (they have different texcoords)
		std::vector<float> tmpVertices = ComputeIcosahedronVertices();

		// clear memory of prev arrays
		std::vector<float>().swap(vertex);
		std::vector<float>().swap(normals);
		std::vector<float>().swap(texCoords);
		std::vector<unsigned int>().swap(index);
		std::vector<unsigned int>().swap(lineIndex);
		std::map<std::pair<float, float>, unsigned int>().swap(sharedIndices);

		float v[3];                             // vertex
		float n[3];                             // normal
		float scale;                            // scale factor for normalization

		// smooth icosahedron has 14 non-shared (0 to 13) and
		// 8 shared vertices (14 to 21) (total 22 vertices)
		//  00  01  02  03  04          //
		//  /\  /\  /\  /\  /\          //
		// /  \/  \/  \/  \/  \         //
		//10--14--15--16--17--11        //
		// \  /\  /\  /\  /\  /\        //
		//  \/  \/  \/  \/  \/  \       //
		//  12--18--19--20--21--13      //
		//   \  /\  /\  /\  /\  /       //
		//    \/  \/  \/  \/  \/        //
		//    05  06  07  08  09        //
		// add 14 non-shared vertices first (index from 0 to 13)
		AddVertex(tmpVertices[0], tmpVertices[1], tmpVertices[2]);      // v0 (top)
		AddNormal(0, 0, 1);
		AddTexCoord(S_STEP, 0);
		
		AddVertex(tmpVertices[0], tmpVertices[1], tmpVertices[2]);      // v1
		AddNormal(0, 0, 1);
		AddTexCoord(S_STEP * 3, 0);
		
		AddVertex(tmpVertices[0], tmpVertices[1], tmpVertices[2]);      // v2
		AddNormal(0, 0, 1);
		AddTexCoord(S_STEP * 5, 0);
		
		AddVertex(tmpVertices[0], tmpVertices[1], tmpVertices[2]);      // v3
		AddNormal(0, 0, 1);
		AddTexCoord(S_STEP * 7, 0);
		
		AddVertex(tmpVertices[0], tmpVertices[1], tmpVertices[2]);      // v4
		AddNormal(0, 0, 1);
		AddTexCoord(S_STEP * 9, 0);
		
		AddVertex(tmpVertices[33], tmpVertices[34], tmpVertices[35]);   // v5 (bottom)
		AddNormal(0, 0, -1);
		AddTexCoord(S_STEP * 2, T_STEP * 3);
		
		AddVertex(tmpVertices[33], tmpVertices[34], tmpVertices[35]);   // v6
		AddNormal(0, 0, -1);
		AddTexCoord(S_STEP * 4, T_STEP * 3);
		
		AddVertex(tmpVertices[33], tmpVertices[34], tmpVertices[35]);   // v7
		AddNormal(0, 0, -1);
		AddTexCoord(S_STEP * 6, T_STEP * 3);
		
		AddVertex(tmpVertices[33], tmpVertices[34], tmpVertices[35]);   // v8
		AddNormal(0, 0, -1);
		AddTexCoord(S_STEP * 8, T_STEP * 3);
	
		AddVertex(tmpVertices[33], tmpVertices[34], tmpVertices[35]);   // v9
		AddNormal(0, 0, -1);
		AddTexCoord(S_STEP * 10, T_STEP * 3);

		v[0] = tmpVertices[3];  v[1] = tmpVertices[4];  v[2] = tmpVertices[5];  // v10 (left)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(0, T_STEP);
		
		AddVertex(v[0], v[1], v[2]);                                            // v11 (right)
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 10, T_STEP);

		v[0] = tmpVertices[18]; v[1] = tmpVertices[19]; v[2] = tmpVertices[20]; // v12 (left)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP, T_STEP * 2);
		
		AddVertex(v[0], v[1], v[2]);                                            // v13 (right)
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 11, T_STEP * 2);

		// add 8 shared vertices to array (index from 14 to 21)
		v[0] = tmpVertices[6];  v[1] = tmpVertices[7];  v[2] = tmpVertices[8];  // v14 (shared)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 2, T_STEP);
		sharedIndices[std::make_pair(S_STEP * 2, T_STEP)] = texCoords.size() / 2 - 1;

		v[0] = tmpVertices[9];  v[1] = tmpVertices[10]; v[2] = tmpVertices[11]; // v15 (shared)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 4, T_STEP);
		sharedIndices[std::make_pair(S_STEP * 4, T_STEP)] = texCoords.size() / 2 - 1;

		v[0] = tmpVertices[12]; v[1] = tmpVertices[13]; v[2] = tmpVertices[14]; // v16 (shared)
		scale = ComputeScaleForLength(v, 1);
		n[0] = v[0] * scale;    n[1] = v[1] * scale;    n[2] = v[2] * scale;
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 6, T_STEP);
		sharedIndices[std::make_pair(S_STEP * 6, T_STEP)] = texCoords.size() / 2 - 1;

		v[0] = tmpVertices[15]; v[1] = tmpVertices[16]; v[2] = tmpVertices[17]; // v17 (shared)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 8, T_STEP);
		sharedIndices[std::make_pair(S_STEP * 8, T_STEP)] = texCoords.size() / 2 - 1;

		v[0] = tmpVertices[21]; v[1] = tmpVertices[22]; v[2] = tmpVertices[23]; // v18 (shared)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 3, T_STEP * 2);
		sharedIndices[std::make_pair(S_STEP * 3, T_STEP * 2)] = texCoords.size() / 2 - 1;

		v[0] = tmpVertices[24]; v[1] = tmpVertices[25]; v[2] = tmpVertices[26]; // v19 (shared)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 5, T_STEP * 2);
		sharedIndices[std::make_pair(S_STEP * 5, T_STEP * 2)] = texCoords.size() / 2 - 1;

		v[0] = tmpVertices[27]; v[1] = tmpVertices[28]; v[2] = tmpVertices[29]; // v20 (shared)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 7, T_STEP * 2);
		sharedIndices[std::make_pair(S_STEP * 7, T_STEP * 2)] = texCoords.size() / 2 - 1;

		v[0] = tmpVertices[30]; v[1] = tmpVertices[31]; v[2] = tmpVertices[32]; // v21 (shared)
		ComputeVertexNormal(v, n);
		AddVertex(v[0], v[1], v[2]);
		AddNormal(n[0], n[1], n[2]);
		AddTexCoord(S_STEP * 9, T_STEP * 2);
		sharedIndices[std::make_pair(S_STEP * 9, T_STEP * 2)] = texCoords.size() / 2 - 1;

		// build index list for icosahedron (20 triangles)
		AddIndex(0, 10, 14);      // 1st row (5 tris)
		AddIndex(1, 14, 15);
		AddIndex(2, 15, 16);
		AddIndex(3, 16, 17);
		AddIndex(4, 17, 11);
		AddIndex(10, 12, 14);      // 2nd row (10 tris)
		AddIndex(12, 18, 14);
		AddIndex(14, 18, 15);
		AddIndex(18, 19, 15);
		AddIndex(15, 19, 16);
		AddIndex(19, 20, 16);
		AddIndex(16, 20, 17);
		AddIndex(20, 21, 17);
		AddIndex(17, 21, 11);
		AddIndex(21, 13, 11);
		AddIndex(5, 18, 12);      // 3rd row (5 tris)
		AddIndex(6, 19, 18);
		AddIndex(7, 20, 19);
		AddIndex(8, 21, 20);
		AddIndex(9, 13, 21);

		// add edge lines of icosahedron
		lineIndex.push_back(0);   lineIndex.push_back(10);       // 00 - 10
		lineIndex.push_back(1);   lineIndex.push_back(14);       // 01 - 14
		lineIndex.push_back(2);   lineIndex.push_back(15);       // 02 - 15
		lineIndex.push_back(3);   lineIndex.push_back(16);       // 03 - 16
		lineIndex.push_back(4);   lineIndex.push_back(17);       // 04 - 17
		lineIndex.push_back(10);  lineIndex.push_back(14);       // 10 - 14
		lineIndex.push_back(14);  lineIndex.push_back(15);       // 14 - 15
		lineIndex.push_back(15);  lineIndex.push_back(16);       // 15 - 16
		lineIndex.push_back(16);  lineIndex.push_back(17);       // 10 - 14
		lineIndex.push_back(17);  lineIndex.push_back(11);       // 17 - 11
		lineIndex.push_back(10);  lineIndex.push_back(12);       // 10 - 12
		lineIndex.push_back(12);  lineIndex.push_back(14);       // 12 - 14
		lineIndex.push_back(14);  lineIndex.push_back(18);       // 14 - 18
		lineIndex.push_back(18);  lineIndex.push_back(15);       // 18 - 15
		lineIndex.push_back(15);  lineIndex.push_back(19);       // 15 - 19
		lineIndex.push_back(19);  lineIndex.push_back(16);       // 19 - 16
		lineIndex.push_back(16);  lineIndex.push_back(20);       // 16 - 20
		lineIndex.push_back(20);  lineIndex.push_back(17);       // 20 - 17
		lineIndex.push_back(17);  lineIndex.push_back(21);       // 17 - 21
		lineIndex.push_back(21);  lineIndex.push_back(11);       // 21 - 11
		lineIndex.push_back(12);  lineIndex.push_back(18);       // 12 - 18
		lineIndex.push_back(18);  lineIndex.push_back(19);       // 18 - 19
		lineIndex.push_back(19);  lineIndex.push_back(20);       // 19 - 20
		lineIndex.push_back(20);  lineIndex.push_back(21);       // 20 - 21
		lineIndex.push_back(21);  lineIndex.push_back(13);       // 21 - 13
		lineIndex.push_back(5);   lineIndex.push_back(12);       // 05 - 12
		lineIndex.push_back(6);   lineIndex.push_back(18);       // 06 - 18
		lineIndex.push_back(7);   lineIndex.push_back(19);       // 07 - 19
		lineIndex.push_back(8);   lineIndex.push_back(20);       // 08 - 20
		lineIndex.push_back(9);   lineIndex.push_back(21);       // 09 - 21

		// subdivide icosahedron
		SubdivideVerticesSmooth();

		// generate interleaved vertex array as well
		BuildInterleavedVertices();
	}
	void SubdivideVerticesFlat()
	{
		std::vector<float> tmpVertices;
		std::vector<float> tmpTexCoords;
		std::vector<unsigned int> tmpIndices;
		int indexCount;
		const float* v1, * v2, * v3;          // ptr to original vertices of a triangle
		const float* t1, * t2, * t3;          // ptr to original texcoords of a triangle
		float newV1[3], newV2[3], newV3[3]; // new vertex positions
		float newT1[2], newT2[2], newT3[2]; // new texture coords
		float normal[3];                    // new face normal
		unsigned int indexs = 0;             // new index value
		int i, j;

		// iteration
		for (i = 1; i <= subdivision; ++i)
		{
			// copy prev arrays
			tmpVertices = vertex;
			tmpTexCoords = texCoords;
			tmpIndices = index;

			// clear prev arrays
			vertex.clear();
			normals.clear();
			texCoords.clear();
			index.clear();
			lineIndex.clear();

			indexs = 0;
			indexCount = (int)tmpIndices.size();
			for (j = 0; j < indexCount; j += 3)
			{
				// get 3 vertice and texcoords of a triangle
				v1 = &tmpVertices[tmpIndices[j] * 3];
				v2 = &tmpVertices[tmpIndices[j + 1] * 3];
				v3 = &tmpVertices[tmpIndices[j + 2] * 3];
				t1 = &tmpTexCoords[tmpIndices[j] * 2];
				t2 = &tmpTexCoords[tmpIndices[j + 1] * 2];
				t3 = &tmpTexCoords[tmpIndices[j + 2] * 2];

				// get 3 new vertices by spliting half on each edge
				ComputeHalfVertex(v1, v2, radius, newV1);
				ComputeHalfVertex(v2, v3, radius, newV2);
				ComputeHalfVertex(v1, v3, radius, newV3);
				ComputeHalfTexCoord(t1, t2, newT1);
				ComputeHalfTexCoord(t2, t3, newT2);
				ComputeHalfTexCoord(t1, t3, newT3);

				// add 4 new triangles
				AddVertices(v1, newV1, newV3);
				AddTexCoords(t1, newT1, newT3);
				ComputeFaceNormal(v1, newV1, newV3, normal);
				AddNormals(normal, normal, normal);
				AddIndex(indexs, indexs + 1, indexs + 2);
				
				AddVertices(newV1, v2, newV2);
				AddTexCoords(newT1, t2, newT2);
				ComputeFaceNormal(newV1, v2, newV2, normal);
				AddNormals(normal, normal, normal);
				AddIndex(indexs + 3, indexs + 4, indexs + 5);
				
				AddVertices(newV1, newV2, newV3);
				AddTexCoords(newT1, newT2, newT3);
				ComputeFaceNormal(newV1, newV2, newV3, normal);
				AddNormals(normal, normal, normal);
				AddIndex(indexs + 6, indexs + 7, indexs + 8);
				
				AddVertices(newV3, newV2, v3);
				AddTexCoords(newT3, newT2, t3);
				ComputeFaceNormal(newV3, newV2, v3, normal);
				AddNormals(normal, normal, normal);
				AddIndex(indexs + 9, indexs + 10, indexs + 11);

				// add new line indices per iteration
				AddSubLineIndex(indexs, indexs + 1, indexs + 4, indexs + 5, indexs + 11, indexs + 9); //CCW

				// next index
				indexs += 12;
			}
		}
	}
	void SubdivideVerticesSmooth()
	{
		std::vector<unsigned int> tmpIndices;
		int indexCount;
		unsigned int i1, i2, i3;            // indices from original triangle
		const float* v1, * v2, * v3;          // ptr to original vertices of a triangle
		const float* t1, * t2, * t3;          // ptr to original texcoords of a triangle
		float newV1[3], newV2[3], newV3[3]; // new subdivided vertex positions
		float newN1[3], newN2[3], newN3[3]; // new subdivided normals
		float newT1[2], newT2[2], newT3[2]; // new subdivided texture coords
		unsigned int newI1, newI2, newI3;   // new subdivided indices
		int i, j;

		// iteration for subdivision
		for (i = 1; i <= subdivision; ++i)
		{
			// copy prev indices
			tmpIndices = index;

			// clear prev arrays
			index.clear();
			lineIndex.clear();

			indexCount = (int)tmpIndices.size();
			for (j = 0; j < indexCount; j += 3)
			{
				// get 3 indices of each triangle
				i1 = tmpIndices[j];
				i2 = tmpIndices[j + 1];
				i3 = tmpIndices[j + 2];

				// get 3 vertex attribs from prev triangle
				v1 = &vertex[i1 * 3];
				v2 = &vertex[i2 * 3];
				v3 = &vertex[i3 * 3];
				t1 = &texCoords[i1 * 2];
				t2 = &texCoords[i2 * 2];
				t3 = &texCoords[i3 * 2];

				// get 3 new vertex attribs by spliting half on each edge
				ComputeHalfVertex(v1, v2, radius, newV1);
				ComputeHalfVertex(v2, v3, radius, newV2);
				ComputeHalfVertex(v1, v3, radius, newV3);
				ComputeHalfTexCoord(t1, t2, newT1);
				ComputeHalfTexCoord(t2, t3, newT2);
				ComputeHalfTexCoord(t1, t3, newT3);
				ComputeVertexNormal(newV1, newN1);
				ComputeVertexNormal(newV2, newN2);
				ComputeVertexNormal(newV3, newN3);

				// add new vertices/normals/texcoords to arrays
				// It will check if it is shared/non-shared and return index
				newI1 = AddSubVertexAttribs(newV1, newN1, newT1);
				newI2 = AddSubVertexAttribs(newV2, newN2, newT2);
				newI3 = AddSubVertexAttribs(newV3, newN3, newT3);

				// add 4 new triangle indices
				AddIndex(i1, newI1, newI3);
				AddIndex(newI1, i2, newI2);
				AddIndex(newI1, newI2, newI3);
				AddIndex(newI3, newI2, i3);

				// add new line indices
				AddSubLineIndex(i1, newI1, i2, newI2, i3, newI3); //CCW
			}
		}
	}
	void BuildInterleavedVertices()
	{
		std::vector<float>().swap(interleavedVertices);

		std::size_t i, j;
		std::size_t count = vertex.size();
		for (i = 0, j = 0; i < count; i += 3, j += 2)
		{
			interleavedVertices.push_back(vertex[i]);
			interleavedVertices.push_back(vertex[i + 1]);
			interleavedVertices.push_back(vertex[i + 2]);

			interleavedVertices.push_back(normals[i]);
			interleavedVertices.push_back(normals[i + 1]);
			interleavedVertices.push_back(normals[i + 2]);

			interleavedVertices.push_back(texCoords[j]);
			interleavedVertices.push_back(texCoords[j + 1]);
		}
	}
	void AddVertex(float x, float y, float z)
	{
		vertex.push_back(x);
		vertex.push_back(y);
		vertex.push_back(z);
	}
	void AddVertices(const float v1[3], const float v2[3], const float v3[3])
	{
		vertex.push_back(v1[0]);  // x
		vertex.push_back(v1[1]);  // y
		vertex.push_back(v1[2]);  // z
		vertex.push_back(v2[0]);
		vertex.push_back(v2[1]);
		vertex.push_back(v2[2]);
		vertex.push_back(v3[0]);
		vertex.push_back(v3[1]);
		vertex.push_back(v3[2]);
	}
	void AddNormal(float nx, float ny, float nz)
	{
		normals.push_back(nx);
		normals.push_back(ny);
		normals.push_back(nz);
	}
	void AddNormals(const float n1[3], const float n2[3], const float n3[3])
	{
		normals.push_back(n1[0]);  // nx
		normals.push_back(n1[1]);  // ny
		normals.push_back(n1[2]);  // nz
		normals.push_back(n2[0]);
		normals.push_back(n2[1]);
		normals.push_back(n2[2]);
		normals.push_back(n3[0]);
		normals.push_back(n3[1]);
		normals.push_back(n3[2]);
	}
	void AddTexCoord(float s, float t)
	{
		texCoords.push_back(s);
		texCoords.push_back(t);
	}
	void AddTexCoords(const float t1[2], const float t2[2], const float t3[2])
	{
		texCoords.push_back(t1[0]); // s
		texCoords.push_back(t1[1]); // t
		texCoords.push_back(t2[0]);
		texCoords.push_back(t2[1]);
		texCoords.push_back(t3[0]);
		texCoords.push_back(t3[1]);
	}
	void AddIndex(unsigned int i1, unsigned int i2, unsigned int i3)
	{
		index.push_back(i1);
		index.push_back(i2);
		index.push_back(i3);
	}
	void AddSubLineIndex(unsigned int i1, unsigned int i2, unsigned int i3,
		unsigned int i4, unsigned int i5, unsigned int i6)
	{
		lineIndex.push_back(i1);      // i1 - i2
		lineIndex.push_back(i2);
		lineIndex.push_back(i2);      // i2 - i6
		lineIndex.push_back(i6);
		lineIndex.push_back(i2);      // i2 - i3
		lineIndex.push_back(i3);
		lineIndex.push_back(i2);      // i2 - i4
		lineIndex.push_back(i4);
		lineIndex.push_back(i6);      // i6 - i4
		lineIndex.push_back(i4);
		lineIndex.push_back(i3);      // i3 - i4
		lineIndex.push_back(i4);
		lineIndex.push_back(i4);      // i4 - i5
		lineIndex.push_back(i5);
	}
	unsigned int AddSubVertexAttribs(const float v[3], const float n[3], const float t[2])
	{
		unsigned int index;     // return value;

		// check if is shared vertex or not first
		if (IsSharedTexCoord(t))
		{
			// find if it does already exist in sharedIndices map using (s,t) key
			// if not in the list, add the vertex attribs to arrays and return its index
			// if exists, return the current index
			std::pair<float, float> key = std::make_pair(t[0], t[1]);
			std::map<std::pair<float, float>, unsigned int>::iterator iter = sharedIndices.find(key);
			if (iter == sharedIndices.end())
			{
				AddVertex(v[0], v[1], v[2]);
				AddNormal(n[0], n[1], n[2]);
				AddTexCoord(t[0], t[1]);
				index = texCoords.size() / 2 - 1;
				sharedIndices[key] = index;
			}
			else
			{
				index = iter->second;
			}
		}
		// not shared
		else
		{
			AddVertex(v[0], v[1], v[2]);
			AddNormal(n[0], n[1], n[2]);
			AddTexCoord(t[0], t[1]);
			index = texCoords.size() / 2 - 1;
		}

		return index;
	}

	// memeber vars
	float radius = 1.0f;
	int subdivision = 1;
	std::vector<float> vertex;
	std::vector<float> normals;
	std::vector<float> texCoords;
	std::vector<unsigned int> index;
	std::vector<unsigned int> lineIndex;
	std::map<std::pair<float, float>, unsigned int> sharedIndices;

	// interleaved
	std::vector<float> interleavedVertices;
	int interleavedStride = 0;
};