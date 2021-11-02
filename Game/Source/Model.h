#ifndef __MODEL_H__
#define __MODEL_H__

#include "Globals.h"
#include "Mesh.h"
#include "Shapes3D.h"
#include <vector>
#include <string>

#include "External/assimp/include/cimport.h"
#include "External/assimp/include/scene.h"
#include "External/assimp/include/postprocess.h"

#pragma comment (lib, "Source/External/assimp/libx86/assimp.lib")

class Mesh;

using namespace std;

class  Model
{
public:

	Model();

	~Model();

	bool LoadModel(const char* path);

	bool Draw();
	void DrawEdges(Mesh* m);

private:
	vector<Mesh*> meshes;
	bool edges = true;

};

#endif // !__MODEL_H__
