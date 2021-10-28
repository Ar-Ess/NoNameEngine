#include "Model.h"

using namespace std;

Model::Model()
{
    struct aiLogStream stream;
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
    aiAttachLogStream(&stream);
}

Model::~Model()
{
}

bool Model::LoadModel(const char* path_file)
{
    bool ret = true;

    string path = "Assets/Models/";
    path += path_file;

    const aiScene* scene = aiImportFile(path.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr && scene->HasMeshes())
        aiReleaseImport(scene);
    else
    {
        LOG("Error loading scene %s", path.c_str());
        ret = false;
    }

    for (int i = 0; i < scene->mNumMeshes; i++)
    {
        const aiMesh* aiMesh = scene->mMeshes[i];
        uint numIndex = 0;
        uint* index = nullptr;
        uint numVertex = aiMesh->mNumVertices;
        float* vertex = new float[numVertex * 3];

        memcpy(vertex, aiMesh->mVertices, sizeof(float) * numVertex * 3);
        LOG("New mesh with %d vertices", numVertex);

        if (aiMesh->HasFaces())
        {
            numIndex = aiMesh->mNumFaces * 3;
            index = new uint[numIndex]; // assume each face is a triangle

            for (uint i = 0; i < aiMesh->mNumFaces; ++i)
            {
                if (aiMesh->mFaces[i].mNumIndices != 3)
                {
                    LOG("WARNING, geometry face with != 3 indices! Not triangle!");
                    ret = false;
                }
                else
                {
                    memcpy(&index[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(uint));
                }
            }
        }

        Mesh* m = new Mesh(vertex, numVertex, index, numIndex);
        meshes.push_back(m);
    }

    aiDetachAllLogStreams();

    return ret;
}

bool Model::Draw()
{
    bool ret = true;
    for (int i = 0; i < meshes.size(); i++)
    {
        Mesh* m = meshes[i];
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->GetIndexID());
        glDrawElements(GL_TRIANGLES, m->GetIndexNumber(), GL_UNSIGNED_INT, NULL);
    }

    return ret;
}
