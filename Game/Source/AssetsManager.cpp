#include <iostream>
#include <direct.h>
#include "AssetsManager.h"

AssetsManager::AssetsManager(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	PHYSFS_init(NULL); // Initialize PhysFS library

	// Create the required path to parse files with PHYSFS
	char basePath[200];

	_getcwd(basePath, sizeof(basePath));

	LOG("Base path is: %s", basePath);
	PHYSFS_init(nullptr);

	if (PHYSFS_setWriteDir(basePath) == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());
	MountPath(basePath);
	MountPath("Assets");
}

AssetsManager::~AssetsManager()
{
	PHYSFS_deinit(); 
}

bool AssetsManager::MountPath(const char* path)
{
	if (PHYSFS_mount(path, nullptr, 1) != 0)
	{
		return true;
	}

	else
	{
		LOG("Error parsing the path!", PHYSFS_getLastError());
		return false;
	}
}

void AssetsManager::ParseFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& directory) const
{
	char** rc = PHYSFS_enumerateFiles(path); // I followed the example in physfs.h and it isn't working :/
	char** i;

	for (i = rc; *i != nullptr; ++i)
	{
		std::string str = std::string(path) + std::string("/") + std::string(*i);
		if (PHYSFS_isDirectory(str.c_str()) != 0)
		{
			directory.push_back(*i);
		}

		else 
		{
			files.push_back(*i);
		}
	}

	PHYSFS_freeList(rc);
}

void AssetsManager::DrawFiles(std::string path)
{
	std::vector<std::string> files;
	std::vector<std::string> directories;

	ParseFiles(path.c_str(), files, directories);

	ImGuiTreeNodeFlags flag = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

	if (directories.empty())
	{
		flag |= ImGuiTreeNodeFlags_Leaf;
	}

	std::string name;
	name = path.substr(path.find_last_of("/") + 1);

	if(ImGui::TreeNodeEx(name.c_str(), flag))
	{
		if (ImGui::IsItemClicked())
		{
			std::string selectedFolder = path;
			LOG("Selected folder: %s", selectedFolder.c_str());
		}

		std::vector<std::string>::iterator it = directories.begin();
		for (; it != directories.end(); it++)
		{
			std::string newPath = path;
			newPath += "/" + (*it);
			DrawFiles(newPath.c_str()); // Recursively call when the path gets updated.
		}

		ImGui::TreePop();
	}
}
