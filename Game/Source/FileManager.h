#define FILEM_EXTENSION ".nne"
#define FILEM_STARTING_POINT 28
#define FILEM_SHAPE_INDEX_POINT 9
#define FILEM_SHAPE_INFO_START_POINT 9

#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include <stdio.h>
#include <string>
#include <iterator>
#include "External/Density/density_api.h"

struct FileEditor;

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

			switch (shape->GetShapeType())
			{
			case MODEL3D:
			{
				Model* m = (Model*)shape;

				fprintf_s(file, "\n>> Shape %d <<", *numberOfShapesSaved);
				fprintf_s(file, "\nMeshes: %d", m->meshes.size());

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
					fprintf_s(file, "%d", tID.channelsPerPixel);

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

		bool Read(unsigned int index, Shape3D* shape)
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

			fopen_s(&file, fileName.c_str(), "r");
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
				if (idx == index)
				{
					int sum = (int)floor((float)index / 10);
					fseek(file, FILEM_SHAPE_INFO_START_POINT + sum, SEEK_CUR);
					break;
				}
				fseek(file, -FILEM_SHAPE_INDEX_POINT, SEEK_CUR);

				int movePos = FindNextChar('>', 2);
				fseek(file, movePos, SEEK_CUR);
			}

			// Checking 
			if (fclose(file) != 0) return false;
			return true;
		}

	private: // Functions
		int FindNextChar(char character, unsigned int offset = 0)
		{
			char a = fgetc(file);
			if (offset != 0) fseek(file, offset - 1, SEEK_CUR);
			char b = fgetc(file);
			int end = (size - ftell(file));

			for (int i = 0; i < end; i++)
			{
				if (fgetc(file) == character) return i;
				fseek(file, 1, SEEK_CUR);
			}

			return -1;
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