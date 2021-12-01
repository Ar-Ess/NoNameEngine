#define FILE_EXTENSION ".nne"

#ifndef __FILE_MANAGER__
#define __FILE_MANAGER__

#include <stdio.h>
#include <string>
#include <vector>

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

				fprintf(file, "\n-- Shape %d --", *numberOfShapesSaved);

				for (int i = 0; i < m->meshes.size(); i++)
				{
					Mesh* mh = m->meshes[i];

					fprintf(file, "\nMesh %d\n - Vertex:\n", i);
					int a = 0;
					for (float* vertex = mh->GetVertexPtr(); vertex != nullptr; vertex += 1)
					{
						a += 1;
						if (a == 100) break;
						fprintf(file, "%f ", *vertex);
					}
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
		if (file == nullptr)
		{
			fopen_s(&file, name.c_str(), "w");
			if (file != nullptr)
			{
				fprintf(file, "PROJECT FILE: %s\n-------------------------------\n", name.c_str());
				fclose(file);
			}
		}

		return FileEditor(name.c_str(), file, &numberOfShapesSaved);
	}

	bool WriteFile(const char* fileName, const char* text)
	{
		std::string name = fileName;
		name += FILE_EXTENSION;

		FILE* file = nullptr;
		fopen_s(&file, name.c_str(), "w");
		if (file == NULL) return false;

		fclose(file);

		return true;
	}

private:

	int numberOfShapesSaved = 0;
};

#endif // !__FILE_MANAGER__