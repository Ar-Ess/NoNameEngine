#define FILEM_EXTENSION ".nne"
#define FILEM_STARTING_POINT 28
#define FILEM_SHAPE_INDEX_POINT 7
#define FILEM_SHAPE_INFO_START_POINT 4
#define FILEM_MESH_COUNT_POINT 8
#define FILEM_MESH_COUNT_START_POINT 3
//#define FILEM_MESH_INDEX_POINT 7
#define FILEM_MESH_INFO_START_POINT 9

#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include <stdio.h>
#include <string>
#include <iterator>
#include "External/Density/density_api.h"
#include "Shapes3D.h"
#include "Model.h"

class FileManager
{
	enum class FileFunctionCall
	{
		OPEN_FILE_FUNCTION,
		ACCESS_FILE_FUNCTION
	};

	struct FileEditor
	{
		FileEditor(const char* name, FILE* f, int* i, FileFunctionCall ffC, bool exist = true)
		{
			fileName = name;
			file = f;
			numberOfShapesSaved = i;
			exists = exist;
			fileFunction = ffC;
		}

		bool Write(Shape3D* shape)
		{
			if (!exists)
			{
				LOG("FILE DOES NOT EXIST. USE OpenFile() TO CREATE A NEW FILE");
				return false;
			}

			fopen_s(&file, fileName.c_str(), "a");

			fprintf_s(file, "\n>Shape %d<", *numberOfShapesSaved);

			fprintf_s(file, "\nName: %s<", shape->GetName());
			fprintf_s(file, "\nType: %s<", shape->WriteShapeType().c_str());
			//Transform information
			fprintf_s(file, "\nTransform:", *numberOfShapesSaved);
			fprintf_s(file, "\n - Position:");
			Point3D p = shape->GetPosition();
			fprintf_s(file, "\n%f<%f<%f<", p.x, p.y, p.z);
			fprintf_s(file, "\n - Rotation:");
			Rotation r = shape->GetRotation();
			fprintf_s(file, "\n%f<%f<%f<%f<", r.angle, r.x, r.y, r.z);
			fprintf_s(file, "\n - Scale:");
			Point3D s = shape->GetScale();
			fprintf_s(file, "\n%f<%f<%f<", s.x, s.y, s.z);

			switch (shape->GetShapeType())
			{
			case MODEL3D:
			{
				Model* m = (Model*)shape;

				fprintf_s(file, "\nMeshes: %d<", m->meshes.size());

				for (int i = 0; i < m->meshes.size(); i++)
				{
					Mesh* mh = m->meshes[i];

					// Vertex
					fprintf_s(file, "\nMesh %d<\n - Vertex: %d<\n", i, mh->GetNum(MeshData::VERTEX));
					for (struct { int a; float* v; void Add() {++a; v += 1;}; } s = { 0, mh->GetVertexPtr() };
						s.a < mh->GetNum(MeshData::VERTEX);
						s.Add())
					{
						fprintf_s(file, "%f<", *s.v);
					}

					// Index
					fprintf_s(file, "\n - Index: %d<\n", mh->GetNum(MeshData::INDEX));
					for (struct { int a; uint* idx; void Add() { ++a; idx += 1; }; } s = { 0, mh->GetIndexPtr() };
						s.a < mh->GetNum(MeshData::INDEX);
						s.Add())
					{
						fprintf_s(file, "%d<", *s.idx);
					}

					// Normals
					fprintf_s(file, "\n - Normals: %d<\n", mh->GetNum(MeshData::NORMAL));
					for (struct { int a; float* n; void Add() { ++a; n += 1; }; } s = { 0, mh->GetNormalPtr() };
						s.a < mh->GetNum(MeshData::NORMAL);
						s.Add())
					{
						fprintf_s(file, "%f<", *s.n);
					}

					// Texture Coords
					fprintf_s(file, "\n - Texture Coords: %d<\n", mh->GetNum(MeshData::TEXTURE_COORDS));
					for (struct { int a; float* t; void Add() { ++a; t += 1; }; } s = { 0, mh->GetTexturePtr() };
						s.a < mh->GetNum(MeshData::TEXTURE_COORDS);
						s.Add())
					{
						fprintf_s(file, "%f<", *s.t);
					}

					// Texture Internal Data
					fprintf_s(file, "\n - TID: \n"); // Pixels, InternalFormat, Width, Height, TextCoordArraySize in Bytes, ChannelsPerPixel
					TextureInternalData tID = mh->GetInternalData();
					int dataNum = tID.width * tID.height * tID.channelsPerPixel;
					// Miss Pixels
					fprintf_s(file, "%d<", (int)tID.internalFormat);
					fprintf_s(file, "%d<", tID.width);
					fprintf_s(file, "%d<", tID.height);
					fprintf_s(file, "%d<", (int)tID.textCoordArraySizeinBytes);
					fprintf_s(file, "%d<", tID.channelsPerPixel);

					//// Example
					//uint_fast64_t compress_safe_size = density_compress_safe_size(dataNum);
					////uint_fast64_t decompress_safe_size = density_decompress_safe_size(datanum);
					//
					//// Allocate required memory
					//uint8_t* outCompressed = (uint8_t*)malloc(compress_safe_size * sizeof(unsigned char));
					////uint8_t* outDecompressed = (uint8_t*)malloc(decompress_safe_size * sizeof(unsigned char));
					//density_processing_result result;
					//
					//result = density_compress(tID.pixels, dataNum, outCompressed, compress_safe_size, DENSITY_ALGORITHM::DENSITY_ALGORITHM_LION);
					//if (!result.state)
					//	LOG("Compressed %llu bytes to %llu bytes\n", result.bytesRead, result.bytesWritten);
					//
					////// Decompress
					////result = density_decompress(outCompressed, result.bytesWritten, outDecompressed, decompress_safe_size);
					////if (!result.state)
					////	printf("Decompressed %llu bytes to %llu bytes\n", result.bytesRead, result.bytesWritten);
					//
					//// Free memory_allocated
					//free(outCompressed);
					////free(outDecompressed);
					//
					////for (struct { int a;  unsigned char* p; void Add() { ++a; p += 1; }; } s = { 0, tID.pixels };
					////	s.a < dataNum; 
					////	s.Add())
					////{
					////	unsigned char a = *s.p;
					////	fprintf_s(file, "%d ", *s.p);
					////}
				}
				fprintf_s(file, "\nMeshes:<");

				break;
			}
			}

			*numberOfShapesSaved += 1;

			if (fclose(file) != 0) return false;
			return true;
		}

		bool Read(unsigned int index, Shape3D** s)
		{
			if (!exists)
			{
				LOG("FILE DOES NOT EXIST. USE OpenFile() TO CREATE A NEW FILE");
				return false;
			}
			if (fileFunction == FileFunctionCall::OPEN_FILE_FUNCTION)
			{
				LOG("CAN NOT READ AN OPENED FILE, USE AccessFile(). OpenFile() GENERATES A NEW BLANK FILE, CAN NOT READ SOMETHING EMPTY");
				return false;
			}
			if (index > *numberOfShapesSaved)
			{
				LOG("NO SHAPE EXIST IN THE INDEX GIVEN");
				return false;
			}

			// Open the file to read
			fopen_s(&file, fileName.c_str(), "r");

			// Find the total size
			fseek(file, 0, SEEK_END);
			size = ftell(file);
			fseek(file, 0, SEEK_SET);

			// Setting file to starting point to read
			fseek(file, FILEM_STARTING_POINT + 2, SEEK_SET);

			// Search for the index shape setted
			for (int i = 0; i < *numberOfShapesSaved; i++)
			{
				fseek(file, FILEM_SHAPE_INDEX_POINT, SEEK_CUR);
				int idx = -1;
				fscanf_s(file, "%d", &idx);
				fseek(file, -1, SEEK_CUR);
				if (idx == index)
				{
					int sum = (int)floor((float)index / 10);
					fseek(file, FILEM_SHAPE_INFO_START_POINT + sum, SEEK_CUR);
					break;
				}
				fseek(file, -FILEM_SHAPE_INDEX_POINT, SEEK_CUR);
				if (!GoNextChar('>', 1))
				{
					LOG("DID NOT FIND ANY CHARACTER AS SPECIFIED IN THE GIVEN FILE");
					return false;
				}
			}

			// Read shape name
			fseek(file, 6, SEEK_CUR); // Advance "Name: "
			int n = FindNextChar('<');
			char name[300] = {};
			fread_s(name, n + 1, n, sizeof(char), file);
			fseek(file, 3, SEEK_CUR);

			// Read shape type
			fseek(file, 6, SEEK_CUR); // Advance "Type: "
			n = FindNextChar('<');
			char type[12] = {};
			fread_s(type, n + 1, n, sizeof(char), file);
			fseek(file, 3, SEEK_CUR);

			// Initialize shape
			Shape3D* shape = nullptr;
			ShapeType sT = shape->ReadShapeType(type);
			switch (sT)
			{
			case CUBE3D: shape = new Cube3D(); break;
			case LINE3D: shape = new Line3D(); break;
			case PYRAMID3D: shape = new Pyramid3D(); break;
			case CYLINDER3D: shape = new Cylinder3D(); break;
			case PLANE3D: shape = new Plane3D(); break;
			case SPHERE3D: shape = new Sphere3D(); break;
			case MODEL3D: shape = new Model(); break;
			}

			shape->SetName(name);

			// Read transform info
			fseek(file, 12, SEEK_CUR); // Advance "Transform: "

			// Read position info
			fseek(file, 14, SEEK_CUR); // Advance " - Position: "
			Point3D point = {};
			fscanf_s(file, "%f<%f<%f", &point.x, &point.y, &point.z);
			shape->SetPosition(point);
			fseek(file, 3, SEEK_CUR);

			// Read rotation info
			fseek(file, 14, SEEK_CUR); // Advance " - Rotation: "
			Rotation rot = {};
			fscanf_s(file, "%f<%f<%f<%f", &rot.angle, &rot.x, &rot.y, &rot.z);
			shape->SetRotation(rot);
			fseek(file, 3, SEEK_CUR);

			// Read scale info
			fseek(file, 11, SEEK_CUR); // Advance " - Scale: "
			fscanf_s(file, "%f<%f<%f", &point.x, &point.y, &point.z);
			shape->SetScale(point);
			fseek(file, 3, SEEK_CUR);

			// Read number of meshes
			Model* m = (Model*)shape;
			fseek(file, FILEM_MESH_COUNT_POINT, SEEK_CUR);
			int meshSize = -1;
			fscanf_s(file, "%d", &meshSize);
			fseek(file, FILEM_MESH_COUNT_START_POINT, SEEK_CUR);

			for (int i = 0; i < meshSize; i++)
			{
				// Read mesh info
				fseek(file, FILEM_MESH_INFO_START_POINT + 11, SEEK_CUR); // MESH INFO START POINT + ADVANCE " - Vertex: "
				// Read vertex
				int vertexsSize = -1;
				fscanf_s(file, "%d", &vertexsSize);
				fseek(file, 3, SEEK_CUR);
				float* vertexs = new float[vertexsSize];
				float* vPtr = vertexs;
				for (int i = 0; i < vertexsSize; i++)
				{
					fscanf_s(file, "%f<", vPtr);
					vPtr += 1;
				}

				// Read index
				fseek(file, 2 + 10, SEEK_CUR); // Advance \n x 2 + ADVANCE " - Index: "
				int indexSize = -1;
				fscanf_s(file, "%d", &indexSize);
				fseek(file, 3, SEEK_CUR);
				uint* indexs = new uint[indexSize];
				uint* iPtr = indexs;
				for (int i = 0; i < indexSize; i++)
				{
					fscanf_s(file, "%u<", iPtr);
					iPtr += 1;
				}

				// Read normals
				fseek(file, 2 + 12, SEEK_CUR); // Advance \n x 2 + ADVANCE " - Normals: "
				int normalsSize = -1;
				fscanf_s(file, "%d", &normalsSize);
				fseek(file, 3, SEEK_CUR);
				float* normals = new float[normalsSize];
				float* nPtr = normals;
				for (int i = 0; i < normalsSize; i++)
				{
					fscanf_s(file, "%f<", nPtr);
					nPtr += 1;
					LOG("%f", normals[i]);
				}

				// Read texture coords
				fseek(file, 2 + 19, SEEK_CUR); // Advance \n x 2 + ADVANCE " - Texture Coords: "
				int texSize = -1;
				fscanf_s(file, "%d", &texSize);
				fseek(file, 3, SEEK_CUR);
				float* texCoords = new float[texSize];
				float* tPtr = texCoords;
				for (int i = 0; i < texSize; i++)
				{
					fscanf_s(file, "%f<", tPtr);
					tPtr += 1;
				}

				// Read TID
				fseek(file, 2 + 7 + 2, SEEK_CUR); // Advance \n x 2 + ADVANCE " - TID:" + Advance \n x 2
				TextureInternalData tID = {};
				tID.pixels = nullptr;
				fscanf_s(file, "%d<", &tID.internalFormat);
				fscanf_s(file, "%d<", &tID.width);
				fscanf_s(file, "%d<", &tID.height);
				fscanf_s(file, "%d<", &tID.textCoordArraySizeinBytes);
				fscanf_s(file, "%d", &tID.channelsPerPixel);

				m->meshes.push_back(new Mesh(vertexs, (uint)vertexsSize, indexs, (uint)indexSize, normals, (uint)normalsSize, texCoords, (uint)texSize, tID));

				if (i < (meshSize - 1)) fseek(file, 3, SEEK_CUR);
			}

			*s = shape;

			// Close the file
			if (fclose(file) != 0) return false;
			return true;
		}

	private: // Functions
		bool GoNextChar(char character, unsigned int offset = 0)
		{
			if (offset != 0) fseek(file, offset, SEEK_CUR);

			int end = (size - ftell(file));

			for (int i = 0; i < end; i++)
			{
				if (fgetc(file) == character)
				{
					fseek(file, -1, SEEK_CUR);
					return true;
				}
			}

			fseek(file, -(end + 1), SEEK_CUR);
			return false;
		}

		int FindNextChar(char character, unsigned int offset = 0)
		{
			if (offset != 0) fseek(file, offset, SEEK_CUR);
			int pos = ftell(file);
			int end = (size - pos);

			for (int i = 0; i < end; i++)
			{
				if (fgetc(file) == character)
				{
					fseek(file, pos, SEEK_SET);
					return i;
				}
			}

			fseek(file, -(end + 1), SEEK_CUR);
			return -1;
		}

		char DebugChar()
		{
			char c = fgetc(file);
			fseek(file, -1, SEEK_CUR);
			return c;
		}

	private: // Variables
		std::string fileName;
		FILE* file;
		int* numberOfShapesSaved = nullptr;
		int size = 0;
		bool exists = true;
		FileFunctionCall fileFunction;
	};

public:

	// Opens an new access to file to write or read.
	// Checks if the file exists:
	//    - True: Removes the file named "fileName" and generates a new file with "fileName" as name
	//    - False: Generates a new file with "fileName" as name
	FileEditor OpenFile(const char* fileName)
	{
		std::string name = fileName;
		name += FILEM_EXTENSION;

		FILE* file = nullptr;
		fopen_s(&file, name.c_str(), "r");

		if (file != nullptr)
		{
			fclose(file);
			remove(name.c_str());
		}

		fopen_s(&file, name.c_str(), "w");
		fprintf_s(file, "PROJECT FILE:\n-------------");
		fclose(file);

		numberOfShapesSaved = 0;

		return FileEditor(name.c_str(), file, &numberOfShapesSaved, FileFunctionCall::OPEN_FILE_FUNCTION);
	}

	// Opens an existent access to file to write or read.
	// Checks if the file exists:
	//    - True: Appends the new information to the file
	//    - False: Exits any function derivated from it
	FileEditor AccessFile(const char* fileName)
	{
		bool exist = true;
		std::string name = fileName;
		name += FILEM_EXTENSION;

		FILE* file = nullptr;
		fopen_s(&file, name.c_str(), "r");

		if (file == nullptr) exist = false;
		else
			fclose(file);

		return FileEditor(name.c_str(), file, &numberOfShapesSaved, FileFunctionCall::ACCESS_FILE_FUNCTION, exist);
	}

private:

	int numberOfShapesSaved = 0;
};

#endif // !__FILE_MANAGER__