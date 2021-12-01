#define FILE_EXTENSION ".nne"

#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include <stdio.h>
#include <string>
#include <iterator>
#include "External/Density/density_api.h"

struct FileEditor;

class FileManager
{
	struct FileEditor
	{
		FileEditor(const char* name, FILE* f, int* i)
		{
			fileName = name;
			file = f;
			numberOfShapesSaved = i;
		}

		bool Write(Shape3D* shape)
		{
			fopen_s(&file, fileName.c_str(), "a");

			switch (shape->GetShapeType())
			{
			case MODEL3D:
			{
				Model* m = (Model*)shape;

				fprintf_s(file, "-- Shape %d --", *numberOfShapesSaved);

				for (int i = 0; i < m->meshes.size(); i++)
				{
					Mesh* mh = m->meshes[i];

					// Vertex
					fprintf_s(file, "\nMesh %d\n - Vertex: %d\n", i, mh->GetNum(MeshData::VERTEX));
					for (struct { int a; float* v; void Add() {++a; v += 1;}; } s = { 0, mh->GetVertexPtr() };
						s.a < mh->GetNum(MeshData::VERTEX);
						s.Add())
					{
						fprintf_s(file, "%f ", *s.v);
					}

					// Index
					fprintf_s(file, "\n - Index: %d\n", mh->GetNum(MeshData::INDEX));
					for (struct { int a; uint* idx; void Add() { ++a; idx += 1; }; } s = { 0, mh->GetIndexPtr() };
						s.a < mh->GetNum(MeshData::INDEX);
						s.Add())
					{
						fprintf_s(file, "%d ", *s.idx);
					}

					// Normals
					fprintf_s(file, "\n - Normals: %d\n", mh->GetNum(MeshData::NORMAL));
					for (struct { int a; float* n; void Add() { ++a; n += 1; }; } s = { 0, mh->GetNormalPtr() };
						s.a < mh->GetNum(MeshData::NORMAL);
						s.Add())
					{
						fprintf_s(file, "%f ", *s.n);
					}

					// Texture Coords
					fprintf_s(file, "\n - Texture Coords: %d\n", mh->GetNum(MeshData::TEXTURE_COORDS));
					for (struct { int a; float* t; void Add() { ++a; t += 1; }; } s = { 0, mh->GetTexturePtr() };
						s.a < mh->GetNum(MeshData::TEXTURE_COORDS);
						s.Add())
					{
						fprintf_s(file, "%f ", *s.t);
					}

					// Texture Internal Data
					fprintf_s(file, "\n - Texture Internal Data: (Pixels, InternalFormat, Width, Height, TextCoordArraySize in Bytes, ChannelsPerPixel)\n");
					TextureInternalData tID = mh->GetInternalData();
					int dataNum = tID.width * tID.height * tID.channelsPerPixel;
					fprintf_s(file, "%d\n", (int)tID.internalFormat);
					fprintf_s(file, "%d\n", tID.width);
					fprintf_s(file, "%d\n", tID.height);
					fprintf_s(file, "%d\n", (int)tID.textCoordArraySizeinBytes);
					fprintf_s(file, "%d\n", tID.channelsPerPixel);
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

				break;
			}
			}

			*numberOfShapesSaved += 1;

			if (fclose(file) != 0) return false;

			return true;
		}

	private:
		std::string fileName;
		FILE* file;
		int* numberOfShapesSaved = nullptr;
	};

public:

	FileEditor OpenFile(const char* fileName)
	{
		std::string name = fileName;
		name += FILE_EXTENSION;

		FILE* file = nullptr;
		fopen_s(&file, name.c_str(), "r");

		if (file != nullptr)
		{
			fclose(file);
			remove(name.c_str());
		}

		fopen_s(&file, name.c_str(), "w");
		fprintf_s(file, "PROJECT FILE: %s\n-------------------------------\n", name.c_str());
		fclose(file);

		return FileEditor(name.c_str(), file, &numberOfShapesSaved);
	}

private:

	int numberOfShapesSaved = 0;
};

#endif // !__FILE_MANAGER__