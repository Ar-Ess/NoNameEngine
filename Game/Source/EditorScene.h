#pragma once
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleRender.h"
#include "AssetsManager.h"
#include "ImportManager.h"
#include "FileManager.h"
#include "Model.h"
#include "Globals.h"
#include <string>

#include "../Source/External/ImGui/imgui.h"
#include "../Source/External/ImGui/imgui_impl_sdl.h"
#include "../Source/External/ImGui/imgui_impl_opengl3.h"
#include "../Source/External/ImGui/imgui_stdlib.h"
#include "External/ImGuizmo-master/ImGuizmo.h"
#include "External/ImGuizmo-master/ImSequencer.h"
#include "External/ImGuizmo-master/ImZoomSlider.h"
#include "External/ImGuizmo-master/ImCurveEdit.h"
#include "External/imgui/imgui_internal.h"
#include "Shapes3D.h"

#include <math.h>
#include <vector>
#include <algorithm>
#include <stdio.h>
#include "External/SDL/include/SDL.h"
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <SDL_opengles2.h>
#else
#include "External/SDL/include/SDL_opengl.h"
#endif

#define YELLOW {1.0f, 1.0f, 0.0f, 1.0f}

class Image;

class EditorScene
{
public:
	EditorScene(Application* app, vector<Shape3D*>* shapes, AssetsManager* assetsManager, ImportManager* importManager, FileManager* fileManager);
	~EditorScene();

	bool Start();
	bool Update();

	bool CleanUp();

private: // Functions
	bool DrawMenuBar();
	bool DrawDocking();

	bool ShowAboutWindow(bool open);
	bool ShowOutputWindow(bool open);
	bool ShowConfigWindow(bool open);
	bool ShowHierarchyWindow(bool open);
	bool ShowInspectorWindow(bool open);
	bool ShowAssetsWindow(bool open);
	bool ShowGameStateWindow(bool open);

	bool ShortCuts();

	// Instantly browse on internet the link written as an argument
	void LinkBrowser(string link)
	{
		ShellExecuteA(NULL, "open", link.c_str(), NULL, NULL, SW_SHOWNORMAL);
	}

	// Input from 0 (smallest spacing) to whatever you need
	void AddSpacing(unsigned int spaces = 1)
	{
		short int plus = 0;
		if (spaces == 0) plus = 1;
		for (int i = 0; i < spaces * 2 + plus; i++) ImGui::Spacing();
	}

	// Input from 1 to whatever you need
	void AddSeparator(unsigned int separator = 1)
	{
		if (separator == 0) return;
		for (int i = 0; i < separator; i++) ImGui::Separator();
	}

	void AddHelper(const char* desc, const char* title = "(?)")
	{
		ImGui::TextDisabled(title);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	// Given an Image enum type, width and height, draw an image
	void AddImage(Image img, unsigned int w = 0, unsigned int h = 0)
	{
		int width = (int)img.GetImageDimensions().x;
		int height = (int)img.GetImageDimensions().y;

		if (w != 0 || h != 0)
		{
			width = w;
			height = h;
		}

		ImGui::Image(ImTextureID(img.GetTextureId()),ImVec2(width, height));
	}

	void AddImage(Image img, int proportion = 100)
	{
		int width = (int)img.GetImageDimensions().x;
		int height = (int)img.GetImageDimensions().y;

		if (proportion <= 0) proportion = 100;

		if (proportion != 100)
		{
			width = int(ceil((width * proportion) / 100));
			height = int(ceil((height * proportion) / 100));
		}

		ImGui::Image(ImTextureID(img.GetTextureId()), ImVec2(width, height));
	}

	// Given an Image enum type, width and height, draw an image
	bool AddImageButton(Image img, unsigned int w = 0, unsigned int h = 0)
	{
		int width = (int)img.GetImageDimensions().x;
		int height = (int)img.GetImageDimensions().y;

		if (w != 0 || h != 0)
		{
			width = w;
			height = h;
		}

		if (ImGui::ImageButton(ImTextureID(img.GetTextureId()), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1), 0)) return true;

		return false;
	}

	bool AddImageButton(Image img, int proportion = 100)
	{
		int width = (int)img.GetImageDimensions().x;
		int height = (int)img.GetImageDimensions().y;

		if (proportion <= 0) proportion = 100;

		if (proportion != 100)
		{
			width = int(ceil((width * proportion) / 100));
			height = int(ceil((height * proportion) / 100));
		}

		if (ImGui::ImageButton(ImTextureID(img.GetTextureId()), ImVec2(width, height), ImVec2(0, 0), ImVec2(1, 1))) return true;

		return false;
	}

	Shape3D* GetShapeFromId(vector<Shape3D*> shapes, int id, int* index = nullptr)
	{
		for (int a = 0; a < shapes.size(); a++)
		{
			if (shapes[a]->id == id)
			{
				if (index != nullptr) *index = a;
				return shapes[a];
			}

			if (!shapes[a]->childs.empty())
			{
				Shape3D* s = GetShapeFromId(shapes[a]->childs, id, index);
				if (s != nullptr) return s;
			}
		}

		return nullptr;
	}

	bool DeleteShapeFromId(vector<Shape3D*>* shapes, int id)
	{
		int size = shapes->size();
		for (int a = 0; a < size; a++)
		{
			if (shapes->at(a)->id == id)
			{
				shapes->at(a)->~Shape3D();
				shapes->erase(shapes->begin() + a);
				if (shapes->size() == 0) shapes->clear();
				return true;
			}

			if (!shapes->at(a)->childs.empty())
			{
				bool deleted = DeleteShapeFromId(&shapes->at(a)->childs, id);
				if (deleted) return true;
			}
		}

		return false;
	}

	// Create a default primitive
	void CreatePrimitive(ShapeType sT, bool asChild = false);

	// Push Back of a Shape3D
	void PushShape3D(Shape3D* s3D, vector<Shape3D*>* vector = nullptr);

	// Duplicates selected shape
	void DuplicateSelectedShape();

	// Pop First of a Shape3D
	void DeleteShape();

	// Pop All Shapes on scene
	void DeleteAllShapes(bool enableMessage = true);

	void DeleteAll(vector<Shape3D*>* shapes, int offset = 0)
	{
		for (int i = (shapes->size() - 1); i >= offset; i--)
		{
			Shape3D* s = shapes->at(i);
			if (!s->childs.empty()) DeleteAll(&s->childs);

			s->~Shape3D();
			shapes->erase(shapes->begin() + i);
		}

		if (offset == 0) shapes->clear();
	}

	// Guizmo functions
	void EditTransform(bool editTransformDecomposition);

	void TravelShapes(vector<Shape3D*>* shapes, int depth = 0);

	void DiselectShapes(vector<Shape3D*> shapes, Shape3D* ref);

	int SetValidId(vector<Shape3D*> s, int size = 0);

public: // Variables
	bool demoWindow = false;
	bool outputWindow = true;
	bool configWindow = false;
	bool hierarchyWindow = true;
	bool inspectorWindow = true;
	bool assetsWindow = true;
	bool gameStateWindow = true;

	bool aboutPopup = false;

	float brightLevel = 1.0f;
	bool wFullScreen = false;
	bool wFullDesktop = false;
	bool wResizable = false;
	bool wBorderless = false;
	bool wVSync = false;
	bool wKeepProportion = false;

	// Geometry View
	bool depthTest = false;
	bool cullFace = false;
	bool lighting = false;
	bool colorMaterial = false;
	bool texture2D = false;

	// Geometry Info
	bool edges = true;
	bool normals = false;
	bool solid = true;

	// Information for Cam
	bool onWindow = false;

	Point wSize = { SCREEN_WIDTH, SCREEN_HEIGHT };
	int wSizeProportion = 100;

	std::string projectName = "New Project";
	std::string teamName = "Team Name";

private: // Variables

	ImVec4 backgroundColor = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	Application* app = nullptr;
	float framerate = 0.0f;
	float milliseconds = 0.0f;
	int assetsWidth = 0;
	vector<Shape3D*>* shapes = nullptr;
	AssetsManager* assets = nullptr;
	ImportManager* import = nullptr;
	FileManager* file = nullptr;
	ImGuiID dockSpaceId = {};
	int selectId = 0;
	int prevSelectId = -1;
	Shape3D* prevShape = nullptr;
	bool createToolTip = false;

	Image folderImage = { ImageTexture::IMG_NO_IMAGE };
	Image fileImage = { ImageTexture::IMG_NO_IMAGE };
};
