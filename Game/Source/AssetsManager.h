#pragma once
#include <iostream>
#include "Module.h"
#include "physfs.h"
#include "Globals.h"
#include "External/imgui/imgui.h"
#include <vector>
#include <string>

class AssetsManager : public Module
{
public:

	AssetsManager(Application * app, bool start_enabled = true);
	~AssetsManager();

public:

	bool MountPath(const char* path);
	void ParseFiles(const char* path, std::vector<std::string>& files, std::vector<std::string>& directory) const;
	void DrawFiles(std::string path);

};
