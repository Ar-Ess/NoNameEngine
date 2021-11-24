#pragma once
#include <iostream>
#include "Module.h"
#include "physfs.h"
#include "Globals.h"
#include "External/imgui/imgui.h"
#include <vector>
#include <string>

enum class AssetType
{
	NO_ASSET_TYPE,
	DIRECTORY,
	FILE
};

struct Asset
{
	Asset(const char* path, AssetType assetType)
	{
		type = assetType;
		name = path;
	}

	AssetType type = AssetType::NO_ASSET_TYPE;
	std::string name;
};

class AssetsManager
{
public:

	AssetsManager(const char* path);
	~AssetsManager();

	bool MountPath(const char* path);
	void ParseFiles();
	void ParseForwardFiles(const char* sumPath);
	void ParseBackwardFiles();
	void SetPath(const char* pathh)
	{
		path.clear();
		path = pathh;
	}
	int GetAssetsNumber()
	{
		return assets.size();
	}
	Asset* GetAssetAt(unsigned int i)
	{
		return assets.at(i);
	}


private:

	std::vector<Asset*> assets;
	std::string path;

};
