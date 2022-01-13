#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "FileManager.h"

using namespace std;

Model::Model(Point3D pos, Point3D s, Rotation rot) : Shape3D(pos, s, rot, MODEL3D, "Model")
{
    struct aiLogStream stream;
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
    aiAttachLogStream(&stream);
}

Model::~Model()
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->~Mesh();
        delete meshes[i];
    }

    DeleteChilds(&childs);

    meshes.clear();
}

bool Model::LoadModel(const char* pathFile, const char* pathTex)
{
    bool ret = true;

    filePath.clear();
    filePath += pathFile;

    texturePath.clear();
    texturePath += pathTex;

    const aiScene* scene = aiImportFile(pathFile, aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr && scene->HasMeshes()) 
        int i = 0;
    else
    {
        LOG("Error loading scene %s", pathFile);
        ret = false;
        return ret;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        // VARIABLES
        if (&scene->mMeshes[i] == nullptr)
        {
            ret = false;
            break;
        }
        const aiMesh* aiMesh = scene->mMeshes[i];

        uint numIndex = 0;
        uint* index = nullptr;
        uint numVertex = aiMesh->mNumVertices;
        uint numNormal = numVertex;
        uint numTexCoord = numVertex;
        float* vertex = new float[numVertex * 3];
        float* normal = new float[numNormal * 3];

        // VERTEX
        memcpy(vertex, aiMesh->mVertices, sizeof(float) * numVertex * 3);
        LOG("Mesh %d with %d vertices", i, numVertex);

        // INDEX
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

        // NORMAL
        memcpy(normal, aiMesh->mNormals, sizeof(float) * numNormal * 3);
        LOG("Mesh %d with %d normals", i, numNormal);

        // TEXTURE
        TextureInternalData tID({});
        int compPerPixel = 0;
        tID.textCoordArraySizeinBytes = numTexCoord * sizeof(float) * 2;
        float* texture = (float*)malloc(tID.textCoordArraySizeinBytes);
        aiVector3D* coords = aiMesh->mTextureCoords[0];
        if (coords != nullptr)
        {
            for (unsigned int i = 0; i < numTexCoord; i++)
            {
                *(texture + i * 2) = coords[i].x;
                *(texture + i * 2 + 1) = 1.0f - coords[i].y; //this coord image is inverted in the library
            }
        }

        tID.pixels = stbi_load(pathTex, &tID.width, &tID.height, &compPerPixel, STBI_rgb);
        tID.internalFormat = GL_RGBA;
        if (compPerPixel == 3) tID.internalFormat = GL_RGB;

        tID.channelsPerPixel = compPerPixel;

        size = {tID.width, tID.height};
  
        // SETTING MESH
        Mesh* m = new Mesh(vertex, numVertex, index, numIndex, normal, numNormal, texture, numTexCoord, tID);
        meshes.push_back(m);

        // Create AABB bounding box
        box.SetNegativeInfinity();
        box.Enclose((float3*)aiMesh->mVertices, aiMesh->mNumVertices);
        // FREE VARIABLES
        //stbi_image_free(tID.pixels);
    }

    aiReleaseImport(scene);
    aiDetachAllLogStreams();

    return ret;
}

bool Model::ImportModel(const char* pathFile)
{
    bool ret = true;

    filePath.clear();
    filePath += "Assets/Models/";
    filePath += pathFile;

    string name;
    name += pathFile;
    name.erase(name.end() - 4, name.end());

    FileManager file;
    file.OpenBinFile(name.c_str()).Init();

    const aiScene* scene = aiImportFile(filePath.c_str(), aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr && scene->HasMeshes())
        int i = 0;
    else
    {
        LOG("Error loading scene %s", pathFile);
        ret = false;
        return ret;
    }

    for (unsigned int i = 0; i < scene->mNumMeshes; i++)
    {
        // VARIABLES
        if (&scene->mMeshes[i] == nullptr)
        {
            ret = false;
            break;
        }
        const aiMesh* aiMesh = scene->mMeshes[i];
        BinaryMesh* binMesh = new BinaryMesh();
        binMesh->numberOfMeshes = (unsigned)scene->mNumMeshes;

        binMesh->vertexSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;//3==x,y,z
        binMesh->vertex = (float*)malloc(binMesh->vertexSizeBytes);
        memcpy(binMesh->vertex, aiMesh->mVertices, binMesh->vertexSizeBytes);

        binMesh->normalsSizeBytes = aiMesh->mNumVertices * sizeof(float) * 3;//3==x,y,z equal vertex
        binMesh->normals = (float*)malloc(binMesh->normalsSizeBytes);
        memcpy(binMesh->normals, aiMesh->mNormals, binMesh->normalsSizeBytes);

        binMesh->textCoordSizeBytes = aiMesh->mNumVertices * sizeof(float) * 2;//3==u,v
        binMesh->textCoords = (float*)malloc(binMesh->textCoordSizeBytes);
        aiVector3D* coords = aiMesh->mTextureCoords[0];
        if (coords != nullptr)
        {
            for (unsigned int i = 0; i < aiMesh->mNumVertices; i++)
            {
                *(binMesh->textCoords + i * 2) = coords[i].x;
                *(binMesh->textCoords + i * 2 + 1) = 1.0f - coords[i].y; //this coord image is inverted in the library
            }
        }

        binMesh->indexSizeBytes = aiMesh->mNumFaces * sizeof(unsigned) * 3; //3==indices/face
        binMesh->index = (unsigned*)malloc(binMesh->indexSizeBytes);
        for (unsigned int i = 0; i < aiMesh->mNumFaces; i++)
        {
            aiFace* f = aiMesh->mFaces + i;
            *(binMesh->index + 0 + i * 3) = f->mIndices[0];
            *(binMesh->index + 1 + i * 3) = f->mIndices[1];
            *(binMesh->index + 2 + i * 3) = f->mIndices[2];
        }

        file.AccessBinFile(name.c_str()).Write(binMesh);

        free(binMesh->vertex);
        free(binMesh->normals);
        free(binMesh->textCoords);
        free(binMesh->index);
        delete binMesh;

        // Create AABB bounding box
        box.SetNegativeInfinity();
        box.Enclose((float3*)aiMesh->mVertices, aiMesh->mNumVertices);
        // FREE VARIABLES
        //stbi_image_free(tID.pixels);
    }

    vector<BinaryMesh*> binMesh;
    file.AccessBinFile(name.c_str()).Read(&binMesh);

    for (unsigned int i = 0; i < binMesh.size(); i++)
    {
        meshes.push_back(file.CreateMeshFromBinary(binMesh[i]));
        delete binMesh[i];
    }

    binMesh.clear();

    aiReleaseImport(scene);
    aiDetachAllLogStreams();

    return ret;
}

bool Model::LoadTexture(const char* pathTex)
{
    bool ret = true;

    texturePath.clear();
    texturePath += pathTex;

    int meshNum = meshes.size();

    for (int i = 0; i < meshNum; i++)
    {
        Mesh* m = meshes[i];
        // TEXTURE
        TextureInternalData tID({});
        int compPerPixel = 0;
        tID.textCoordArraySizeinBytes = m->GetNum(TEXTURE_COORDS) * sizeof(float) * 2;
        tID.pixels = stbi_load(pathTex, &tID.width, &tID.height, &compPerPixel, STBI_rgb);
        tID.internalFormat = GL_RGBA;
        if (compPerPixel == 3) tID.internalFormat = GL_RGB;

        size = { tID.width, tID.height };
        
        Mesh* newM = new Mesh(m->GetVertexPtr(), m->GetNum(VERTEX), 
                              m->GetIndexPtr(), m->GetNum(INDEX), 
                              m->GetNormalPtr(), m->GetNum(NORMAL),
                              m->GetTexturePtr(), m->GetNum(TEXTURE_COORDS), tID);
        meshes.push_back(newM);
    }

    for (int i = 0; i < meshNum; i++) meshes.erase(meshes.begin());

    return ret;
}

bool Model::Draw()
{
    bool ret = true;
    glLineWidth(1.0f);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    if (showTexture) glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    for (unsigned int i = 0; !components.empty() && i < components.size(); i++) components[i]->Update(this);

    glTranslatef(position.x, position.y, position.z);

    glRotatef(rotation.angle, rotation.x, rotation.y, rotation.z);

    glScalef(scale.x, scale.y, scale.z);

    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        Mesh* m = meshes[i];
        if (solid) DrawSolid(m);
        if (edges) DrawEdges(m);
        if (normals) DrawNormals(m);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    if (showTexture) glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    return ret;
}

bool Model::DrawSolid(Mesh* m)
{
    bool ret = true;

    glColor3f(255, 255, 255);

    //vertex
    glBindBuffer(GL_ARRAY_BUFFER, m->GetId(VERTEX));
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    //normals
    glBindBuffer(GL_ARRAY_BUFFER, m->GetId(NORMAL));
    glNormalPointer(GL_FLOAT, 0, NULL);

    if (showTexture)
    {
        //coord
        glBindBuffer(GL_ARRAY_BUFFER, m->GetId(TEXTURE_COORDS));
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);

        //texture
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, m->GetId(TEXTURE));
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->GetId(INDEX));
    glDrawElements(GL_TRIANGLES, m->GetNum(INDEX), GL_UNSIGNED_INT, NULL);

    return ret;
}

void Model::DrawEdges(Mesh* m)
{
    glColor3f(255, 0, 0);
    //vertex
    glBindBuffer(GL_ARRAY_BUFFER, m->GetId(VERTEX));
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glLineWidth(1.5f);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->GetId(INDEX));
    glDrawElements(GL_LINES, m->GetNum(INDEX), GL_UNSIGNED_INT, NULL);
}

void Model::DrawNormals(Mesh* m)
{

    glColor3f(0, 0, 255);
    //vertex
    glBindBuffer(GL_ARRAY_BUFFER, m->GetId(NORMAL));
    glVertexPointer(3, GL_FLOAT, 0, NULL);

    glLineWidth(1.0f);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->GetId(INDEX));
    glDrawElements(GL_LINES, m->GetNum(INDEX), GL_UNSIGNED_INT, NULL);
}

void Model::UpdateBoundingBox(AABB* aabb)
{
    OBB ob = *aabb;
    Quat trans = { position.x, position.y, position.z, 0 };
    ob.Transform(trans);

    aabb->SetNegativeInfinity();
    aabb->Enclose(ob);
}

//void Model::BoundingBox(aiMesh* mesh)
//{
//    box.SetNegativeInfinity();
//    box.Enclose((float3*)mesh->mVertices, mesh->mNumVertices);
//}
