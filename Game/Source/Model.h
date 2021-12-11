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
#include "External/MathGeoLib/Geometry/AABB.h"
#include "External/MathGeoLib/Geometry/OBB.h"
#include "External/MathGeoLib/Math/Quat.h"

#pragma comment (lib, "Source/External/assimp/libx86/assimp.lib")

class Mesh;

using namespace std;

class Model : public Shape3D
{
public:

	Model(Point3D pos = { 0, 0, 0 }, Point3D s = { 1, 1, 1 }, Rotation rot = { 0, 0, 0, 0 });

	~Model();

	bool LoadModel(const char* pathFile, const char* pathTex = "Assets/Textures/default_texture.jpg");
	bool LoadTexture(const char* pathTex = "Assets/Textures/default_texture.jpg");

	bool Draw();
	bool DrawSolid(Mesh* m);
	void DrawEdges(Mesh* m);
	void DrawNormals(Mesh* m);

public:
	Point GetSize() const
	{
		return size;
	}

	void BoundingBox(aiMesh mesh);

	void UpdateBoundingBox(AABB* aabb);

public:
	vector<Mesh*> meshes = {};
	string filePath = {};
	string texturePath = {};
	Mesh* mesh = nullptr;

	bool showTexture = true;

	AABB box;
	OBB obb;

private:
	Point size = {};

};

#endif // !__MODEL_H__

