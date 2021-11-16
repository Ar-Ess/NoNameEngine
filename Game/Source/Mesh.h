#ifndef __MESH_H__
#define __MESH_H__

#include "Application.h"
#include "Globals.h"

struct TextureInternalData
{
	TextureInternalData()
	{
		pixels = nullptr;
		internalFormat = 0;
		width = 0; 
		height = 0;
		textCoordArraySizeinBytes = 0;
	}

	unsigned char* pixels = nullptr;
	GLint internalFormat = 0;
	int width = 0, height = 0;
	size_t textCoordArraySizeinBytes = 0;
};

enum MeshData
{
	VERTEX,
	INDEX,
	NORMAL,
	TEXTURE,
	TEXTURE_COORDS
};

class Mesh
{
public:

	Mesh(float* vertexBuffer, uint numOfVertex, uint* indexBuffer, uint numOfIndex, float* normalBuffer = nullptr, uint numOfNormal = 0, float* tex = nullptr, uint numOfTextureCoords = 0, TextureInternalData tID = {})
	{
		vertexNum = numOfVertex;
		vertex = vertexBuffer;
		indexNum = numOfIndex;
		index = indexBuffer;
		normalNum = numOfNormal;
		normal = normalBuffer;
		texture = tex;
		texCoordNum = numOfTextureCoords;

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
		glGenBuffers(1, &texCoordID);

		glBindBuffer(GL_ARRAY_BUFFER, vertexID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexNum * 3, vertex, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indexNum, index, GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, normalID);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normalNum * 3, normal, GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, texCoordID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, tID.textCoordArraySizeinBytes, texture, GL_STATIC_DRAW);

		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glGenTextures(1, &textureID);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tID.width, tID.height, 0, tID.internalFormat, GL_UNSIGNED_BYTE, tID.pixels);
	}

	~Mesh() 
	{
		delete index;
		index = nullptr;
		glDeleteBuffers(indexNum, &indexID);

		delete vertex;
		vertex = nullptr;
		glDeleteBuffers(vertexNum, &vertexID);

		delete normal;
		normal = nullptr;
		glDeleteBuffers(normalNum, &normalID);

		delete texture;
		texture = nullptr;
		glDeleteBuffers(texCoordNum, &texCoordID);
		glDeleteTextures(1, &textureID); 
	}

	uint GetId(MeshData md)
	{
		uint ret = 0;

		switch (md)
		{
		case VERTEX: ret = vertexID; break;
		case INDEX: ret = indexID; break;
		case NORMAL: ret = normalID; break;
		case TEXTURE: ret = textureID; break;
		case TEXTURE_COORDS: ret = texCoordID; break;
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
		case TEXTURE: ret = 1; break;
		case TEXTURE_COORDS: ret = texCoordNum; break;
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

	float* GetTexturePtr()
	{
		return texture;
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

	uint texCoordID = 0;
	uint textureID = 0;
	uint texCoordNum = 0;
	float* texture = nullptr;

};
#endif // !__MESH_H__

