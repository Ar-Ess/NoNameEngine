#include "Model.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

Model::Model(Point3D pos, float s, Rotation rot) : Shape3D(pos, s, rot, MODEL3D)
{
    struct aiLogStream stream;
    stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
    aiAttachLogStream(&stream);
}

Model::~Model()
{
}

bool Model::LoadModel(const char* pathFile, const char* pathTex)
{
    bool ret = true;

    const aiScene* scene = aiImportFile(pathFile, aiProcessPreset_TargetRealtime_MaxQuality);

    if (scene != nullptr && scene->HasMeshes()) 
        int i = 0;
    else
    {
        LOG("Error loading scene %s", pathFile);
        ret = false;
        return ret;
    }

    for (int i = 0; i < scene->mNumMeshes; i++)
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
        for (int i = 0; i < numTexCoord; i++)
        {
            *(texture + i * 2) = coords[i].x;
            *(texture + i * 2 + 1) = 1.0 - coords[i].y; //this coord image is inverted in the library
        }

        tID.pixels = stbi_load(pathTex, &tID.width, &tID.height, &compPerPixel, STBI_rgb);
        tID.internalFormat = GL_RGBA;
        if (compPerPixel == 3) tID.internalFormat = GL_RGB;

        // SETTING MESH
        Mesh* m = new Mesh(vertex, numVertex, index, numIndex, normal, numNormal, texture, numTexCoord, tID);
        meshes.push_back(m);

        // FREE VARIABLES
        stbi_image_free(tID.pixels);
    }

    aiReleaseImport(scene);
    aiDetachAllLogStreams();

    return ret;
}

bool Model::LoadTexture(const char* pathTex)
{
    bool ret = true;

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

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glTranslatef(position.x, position.y, position.z);

    glScalef(scale, scale, scale);

    for (int i = 0; i < meshes.size(); i++)
    {
        glColor3f(255, 255, 255);
        Mesh* m = meshes[i];

        //vertex
        glBindBuffer(GL_ARRAY_BUFFER, m->GetId(VERTEX));
        glVertexPointer(3, GL_FLOAT, 0, NULL);

        //normals
        glBindBuffer(GL_ARRAY_BUFFER, m->GetId(NORMAL));
        glNormalPointer(GL_FLOAT, 0, NULL);

        //coord
        glBindBuffer(GL_ARRAY_BUFFER, m->GetId(TEXTURE_COORDS));
        glTexCoordPointer(2, GL_FLOAT, 0, NULL);

        //texture
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_2D, m->GetId(TEXTURE));

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m->GetId(INDEX));
        glDrawElements(GL_TRIANGLES, m->GetNum(INDEX), GL_UNSIGNED_INT, NULL);

        if (edges) DrawEdges(m);
        if (normals) DrawNormals(m);
    }

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

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
