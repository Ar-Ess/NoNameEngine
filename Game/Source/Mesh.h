#ifndef __MESH_H__
#define __MESH_H__

#include "Application.h"
#include "Globals.h"

enum MeshData
{
	VERTEX,
	INDEX,
	NORMAL
};

class Mesh
{
public:

	Mesh(float* vertexBuffer, uint numOfVertex, uint* indexBuffer, uint numOfIndex, float* normalBuffer = nullptr, uint numOfNormal = 0)
	{
		vertexNum = numOfVertex;
		vertex = vertexBuffer;
		indexNum = numOfIndex;
		index = indexBuffer;
		normalNum = numOfNormal;
		normal = normalBuffer;

		uint count = 0;
		bool sum = true;
		for (uint i = 0; i < (normalNum * 2); i++)
		{
			float* iterN = normal;
			float* iterV = vertex;

			if (count == 3)
			{
				sum = !sum;
				count = 0;
				if (!sum) iterV -= 3;
			}

			iterN += i;
			iterV += i;
			*iterN += *iterV;

			count++;
		}

		glGenBuffers(1, &vertexID);
		glGenBuffers(1, &indexID);
		glGenBuffers(1, &normalID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNum * 3, vertex, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, index, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalNum * 3, normal, GL_STATIC_DRAW);
	}

	~Mesh() {}

	uint GetId(MeshData md)
	{
		uint ret = 0;

		switch (md)
		{
		case VERTEX: ret = vertexID; break;
		case INDEX: ret = indexID; break;
		case NORMAL: ret = normalID; break;
		}

		return ret;
	}

	uint GetNum(MeshData md)
	{
		uint ret = 0;

		switch (md)
		{
		case VERTEX: ret = vertexNum; break;
		case INDEX: ret = indexNum; break;
		case NORMAL: ret = normalNum; break;
		}

		return ret;
	}

	uint* GetIndexPtr()
	{
		return index;
	}

	float* GetVertexPtr()
	{
		return vertex;
	}

	float* GetNormalPtr()
	{
		return normal;
	}

private:

	uint indexID = 0; // index in VRAM
	uint indexNum = 0;
	uint* index = nullptr;

	uint vertexID = 0; // unique vertex in VRAM
	uint vertexNum = 0;
	float* vertex = nullptr;

	uint normalID = 0;
	uint normalNum = 0;
	float* normal = nullptr;

	uint textureID = 0;
	Point dimensions = {0, 0};
	int compPerPixel = 0;
	unsigned char* pixels = nullptr;
	GLint internalFormat = GL_RGBA;
	uint textureNum = 0;
	float* texture = nullptr;

};
#endif // !__MESH_H__

