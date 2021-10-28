#ifndef __FBX_LOADER_H__
#define __FBX_LOADER_H__

#include "Globals.h"
#include "Mesh.h"
#include <vector>

#include "External/assimp/include/cimport.h"
#include "External/assimp/include/scene.h"
#include "External/assimp/include/postprocess.h"

#pragma comment (lib, "Source/External/assimp/libx86/assimp.lib")

class Mesh;

using namespace std;

class LoaderFBX
{
public:
	LoaderFBX()
	{
		struct aiLogStream stream;
		stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
		aiAttachLogStream(&stream);
	}

	~LoaderFBX()
	{

	}

	vector<Mesh*> Load(const char* path);

	void CleanUp();

	Mesh testMesh;
};

#endif //__FBX_LOADER_H__
