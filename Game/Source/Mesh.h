#ifndef __MESH_H__
#define __MESH_H__

#include "Application.h"
#include "Globals.h"


class Mesh
{
public:

	Mesh(float* vertexBuffer, uint numOfVertex, uint* indexBuffer, uint numOfIndex)
	{
		vertexNum = numOfVertex;
		vertex = vertexBuffer;
		indexNum = numOfIndex;
		index = indexBuffer;

		glGenBuffers(1, &vertexID);
		glGenBuffers(1, &indexID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNum * 3, vertex, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, index, GL_STATIC_DRAW);
	}

	~Mesh() {}

	uint GetIndexID()
	{
		return indexID;
	}

	float GetVertexID()
	{
		return vertexID;
	}

	uint GetIndexNumber()
	{
		return indexNum;
	}

	float GetVertexNumber()
	{
		return vertexNum;
	}

	uint* GetIndexPtr()
	{
		return index;
	}

	float* GetVertexPtr()
	{
		return vertex;
	}

private:

	uint indexID = 0; // index in VRAM
	uint indexNum = 0;
	uint* index = nullptr;

	uint vertexID = 0; // unique vertex in VRAM
	uint vertexNum = 0;
	float* vertex = nullptr;

};
#endif // !__MESH_H__

