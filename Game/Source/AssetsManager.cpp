#include <iostream>
#include <direct.h>
#include "AssetsManager.h"

AssetsManager::AssetsManager(const char* pathh)
{
	PHYSFS_init(NULL); // Initialize PhysFS library

	// Create the required path to parse files with PHYSFS
	char basePath[200];

	_getcwd(basePath, sizeof(basePath));

	path = basePath;

	LOG("Base path is: %s", basePath);
	PHYSFS_init(nullptr);

	if (PHYSFS_setWriteDir(basePath) == 0)
		LOG("File System error while creating write dir: %s\n", PHYSFS_getLastError());

	MountPath(path.c_str());
	path += pathh;
	MountPath(path.c_str());
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

void AssetsManager::ParseFiles()
{
	std::string dir = path.substr(path.find_last_of("/") + 1);

	char** rc = PHYSFS_enumerateFiles(dir.c_str()); // I followed the example in physfs.h and it isn't working :/
	char** i;

	assets.clear();

	for (i = rc; *i != nullptr; ++i)
	{
		std::string str = dir + std::string("/") + std::string(*i);

		if (PHYSFS_isDirectory(str.c_str()) != 0)
		{
			assets.push_back(new Asset(*i, AssetType::DIRECTORY));
		}
		else
		{
			assets.push_back(new Asset(*i, AssetType::FILE));
		}
	}

	PHYSFS_freeList(rc);
}

void AssetsManager::ParseForwardFiles(const char* sumPath)
{
	std::string dir = path.substr(path.find_last_of("/") + 1);
	if (sumPath != nullptr)
	{
		dir += "/";
		dir += sumPath;
		path += "/";
		path += sumPath;
	}

	char** rc = PHYSFS_enumerateFiles(dir.c_str()); 
	char** i;

	assets.clear();

	for (i = rc; *i != nullptr; ++i)
	{
		std::string str = dir + std::string("/") + std::string(*i);

		if (PHYSFS_isDirectory(str.c_str()) != 0)
		{
			assets.push_back(new Asset(*i, AssetType::DIRECTORY));
		}
		else 
		{
			assets.push_back(new Asset(*i, AssetType::FILE));
		}
	}

	PHYSFS_freeList(rc);
}

void AssetsManager::ParseBackwardFiles()
{
	std::string erase = path.substr(path.find_last_of("/") + 1);

	if (SameString(erase.c_str(), "Assets")) return;

	int size = path.size() - erase.size() - 1;
	path = path.erase(size, erase.size() + 1);

	std::string dir = path.substr(path.find_last_of("/") + 1);

	char** rc = PHYSFS_enumerateFiles(dir.c_str());
	char** i;

	assets.clear();

	for (i = rc; *i != nullptr; ++i)
	{
		std::string str = dir + std::string("/") + std::string(*i);

		if (PHYSFS_isDirectory(str.c_str()) != 0)
		{
			assets.push_back(new Asset(*i, AssetType::DIRECTORY));
		}
		else
		{
			assets.push_back(new Asset(*i, AssetType::FILE));
		}
	}

	PHYSFS_freeList(rc);
}