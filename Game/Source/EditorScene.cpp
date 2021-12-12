#include "EditorScene.h"

#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleCamera3D.h"
#include "stb_image.h"
#include "glmath.h"
#include <algorithm>

EditorScene::EditorScene(Application* App, vector<Shape3D*>* s, AssetsManager* assetsManager, ImportManager* importManager, FileManager* fileManager)
{
	this->app = App;
	this->shapes = s;
	this->assets = assetsManager;
	this->import = importManager;
	this->file = fileManager;
}

EditorScene::~EditorScene()
{
}

bool EditorScene::Start()
{
	bool ret = true;
	//ret = file->OpenFile("test").Write((Shape3D*)m);

	assets->ParseFiles();
	import->LoadDefaultImages();

	Plane3D* p = new Plane3D({ 0, 0, 0 }, { 0, 1, 0 }, Point3D(200));
	p->solid = false;
	shapes->push_back(p);

	Model* m = new Model({ 0, 0, 0 }, { 0.05f,0.05f,0.05f }, {-90, 1, 0, 0});
	m->LoadModel("Assets/Models/street_environment_V01.FBX");
	shapes->push_back(m);

	SetValidId(*shapes);

	return ret;
}

bool EditorScene::Update()
{
	bool ret = true;

	onWindow = ImGui::IsAnyItemHovered();
	if (!onWindow) onWindow = ImGui::IsAnyItemActive();

	if (demoWindow) ImGui::ShowDemoWindow(&demoWindow);
	ret = DrawDocking();
	ret = DrawMenuBar();
	aboutPopup = ShowAboutWindow(aboutPopup);
	outputWindow = ShowOutputWindow(outputWindow);
	configWindow = ShowConfigWindow(configWindow);
	hierarchyWindow = ShowHierarchyWindow(hierarchyWindow);
	inspectorWindow = ShowInspectorWindow(inspectorWindow);
	assetsWindow = ShowAssetsWindow(assetsWindow);
	gameStateWindow = ShowGameStateWindow(gameStateWindow);

	ret = ShortCuts();

	return ret;
}

bool EditorScene::CleanUp()
{
	bool ret = true;
	DeleteAllShapes(false);
	return ret;
}

bool EditorScene::DrawMenuBar()
{
	bool ret = true;

	ImGui::BeginMainMenuBar();
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save File", " Ctrl + S"))
				app->Save("NNE_Project_Saving", FileContent::PROJECT);

			if (ImGui::MenuItem("Load File", "Ctrl + L"))
			{
				DeleteAllShapes(false);
				app->Load("NNE_Project_Saving", FileContent::PROJECT);
				SetValidId(*shapes);
			}

			if (ImGui::MenuItem("Exit", " Esc"))
				ret = false;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			if (ImGui::MenuItem("Duplicate", " Ctrl + D"))
				DuplicateSelectedShape();

			AddSeparator(1);

			if (ImGui::MenuItem("Delete", " Supr"))
				DeleteShape();

			if (ImGui::MenuItem("Delete All", " Supr + Shift"))
				DeleteAllShapes();

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Window"))
		{
			if (ImGui::MenuItem("Configuration"))
				configWindow = !configWindow;

			if (ImGui::MenuItem("Output Log"))
				outputWindow = !outputWindow;

			if (ImGui::MenuItem("Hierarchy"))
				hierarchyWindow = !hierarchyWindow;

			if (ImGui::MenuItem("Assets"))
				assetsWindow = !assetsWindow;

			if (ImGui::MenuItem("Inspector"))
				inspectorWindow = !inspectorWindow;

			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Create"))
		{
			if (ImGui::MenuItem("Empty Shape"))
				CreatePrimitive(EMPTY3D);

			if (ImGui::MenuItem("Empty Child"))
				CreatePrimitive(EMPTY3D, true);

			if (ImGui::BeginMenu("Primitives"))
			{
				if (ImGui::MenuItem("Cube"))
					CreatePrimitive(CUBE3D);

				if (ImGui::MenuItem("Line"))
					CreatePrimitive(LINE3D);

				if (ImGui::MenuItem("Pyramid"))
					CreatePrimitive(PYRAMID3D);

				if (ImGui::MenuItem("Cylinder"))
					CreatePrimitive(CYLINDER3D);

				if (ImGui::MenuItem("Sphere"))
					CreatePrimitive(SPHERE3D);

				if (ImGui::MenuItem("Plane"))
					CreatePrimitive(PLANE3D);

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Test Models"))
			{
				if (ImGui::MenuItem("Cube"))
				{
					Model* m = new Model({ 0, 0, 0 }, 1.0f);
					file->AccessBinFile("cube").Generate(m);
					//m->LoadModel("Assets/Models/cube.fbx");
					PushShape3D(m);
				}
				if (ImGui::MenuItem("Warrior"))
				{
					Model* m = new Model({ 0, 0, 0 }, 1.0f);
					file->AccessBinFile("warrior").Generate(m);
					//m->LoadModel("Assets/Models/warrior.FBX");
					PushShape3D(m);
				}
				if (ImGui::MenuItem("Baker House"))
				{
					Model* m = new Model({ 0, 0, 0 }, 1.0f);
					//m->LoadModel("Assets/Models/BakerHouse.fbx", "Assets/Textures/baker_house_texture.png");
					file->AccessBinFile("BakerHouse").Generate(m);
					PushShape3D(m);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::BeginMenu("Geometry"))
			{
				if (ImGui::MenuItem("Solid"))
				{
					solid = !solid;
					app->scene->SetGeometryInfo(SOLID);
				}

				if (ImGui::MenuItem("Edges"))
				{
					edges = !edges;
					app->scene->SetGeometryInfo(EDGES);
				}

				if (ImGui::MenuItem("Normals"))
				{
					normals = !normals;
					app->scene->SetGeometryInfo(NORMALS);
				}
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Help"))
		{
			if (ImGui::MenuItem("Gui Demo"))
				demoWindow = !demoWindow;

			if (ImGui::MenuItem("Repository"))
				LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v");

			if (ImGui::MenuItem("Documentation"))
				LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v/wiki");

			if (ImGui::MenuItem("Download latest"))
				LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v/releases");

			if (ImGui::MenuItem("Report Bug"))
				LinkBrowser("https://github.com/BooStarGamer/Game-Engine-1.0v/issues");

			if (ImGui::MenuItem("About"))
				aboutPopup = !aboutPopup;

			ImGui::EndMenu();
		}
	}
	ImGui::EndMainMenuBar();

	return ret;
}

bool EditorScene::DrawDocking()
{
	bool ret = true;

	ImGuiDockNodeFlags dockspace_flags = (ImGuiDockNodeFlags_PassthruCentralNode);
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->Pos);
	ImGui::SetNextWindowSize(viewport->Size);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

	ImGui::Begin("Docking", (bool*)0, (ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus));
	
	ImGui::PopStyleVar();

	ImGuiIO& io = ImGui::GetIO();
	if (io.ConfigFlags & (ImGuiConfigFlags_DockingEnable))
	{
		ImGuiID dockspaceId = ImGui::GetID("MyDockSpace");
		ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspace_flags);
	}

	ImGui::End();

	return ret;
}

bool EditorScene::ShowAboutWindow(bool open)
{
	if (open)
	{
		ImGui::OpenPopup("About");

		if (ImGui::BeginPopupModal("About", &open))
		{
			if (!onWindow) onWindow = ImGui::IsWindowHovered();

			ImGui::TextColored({ 1,0,0,1 },"NoNameEngine v0.1\n");
			ImGui::Text("The best 3D engine for the best games.\nCreated by:\n\nMarti Buxeda: \n");
			if (ImGui::Button("Github link"))
			{
				LinkBrowser("https://github.com/Ar-Ess");
			}
			ImGui::Text("Victor Jara: \n");
			if (ImGui::Button("Github link"))
			{
				LinkBrowser("https://github.com/Kerali");
			}
			ImGui::Text("Jordi Espriu: \n");
			if (ImGui::Button("Github link"))
			{
				LinkBrowser("https://github.com/LordUnicorn31");
			}

			ImGui::Text("\n3rd Party Libraries used:\n");
			ImGui::BulletText("SDL 2.0.16");
			ImGui::BulletText("Glew 2.2.0");
			ImGui::BulletText("ImGui v1.85");
			ImGui::BulletText("OpenGL 2.0.6");
			ImGui::BulletText("Assimp 5.0.1");
			ImGui::BulletText("JSON 1.9.4");
			ImGui::BulletText("Parson 1.2.1");
			ImGui::BulletText("STB Image v2.27");

			ImGui::Text("\n\nMIT License\n\nCopyright (c) 2021 Marti Buxeda and Victor Jara\n\n");
			
			ImGui::Text("Permission is hereby granted, free of charge, to any person obtaining a copy \nof this software and associated documentation files(the 'Software'), to deal \nin the Software without restriction, including without limitation the rights \nto use, copy, modify, merge, publish, distribute, sublicense, and /or sell \ncopies of the Software, and to permit persons to whom the Software is \nfurnished to do so, subject to the following conditions :\n ");
			ImGui::Text("The above copyright notice and this permission notice shall be included in all\ncopies or substantial portions of the Software.\n\n");
			ImGui::Text("THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\nIMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\nFITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE\nAUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\nLIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\nOUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\nSOFTWARE.\n\n");
			
			if (ImGui::Button("Close"))
			{
				open = false;
			}

			ImGui::EndPopup();
		}
	}

	return open;
}

bool EditorScene::ShowOutputWindow(bool open)
{
	if (open)
	{
		if (ImGui::Begin("Output Log", &open))
		{
			if (!onWindow) onWindow = ImGui::IsWindowHovered();
			for (int i = 0; i < GetOutputSize(); i++)
			{
				ImGui::Text(GetOutputText(i).c_str());
			}
		}
		ImGui::End();
	}

	return open;
}

bool EditorScene::ShowConfigWindow(bool open)
{
	if (open)
	{
		if (ImGui::Begin("Configuration", &open))
		{
			if (!onWindow) onWindow = ImGui::IsWindowHovered();

			if (ImGui::BeginMenu("Options"))
			{
				if (ImGui::MenuItem("Save"))
					app->Save("NNE_Preferences_Saving", FileContent::CONFIG_PREFERENCES);

				if (ImGui::MenuItem("Load"))
					app->Load("NNE_Preferences_Saving", FileContent::CONFIG_PREFERENCES);

				if (ImGui::MenuItem("Default"))
					app->Load("NNE_Preferences_Default", FileContent::CONFIG_PREFERENCES);

				ImGui::EndMenu();
			}
			if (ImGui::BeginTabBar("#tabs"))
			{
				if (ImGui::BeginTabItem("General"))
				{
					if (ImGui::CollapsingHeader("Application"))
						{
							AddSpacing(0);
							ImGui::Text("PROJECT INFORMATION");
							AddSpacing(1);
							ImGui::Text("Project Name");
							ImGui::InputText("New Project", &projectName);
							if (!onWindow) onWindow = ImGui::IsItemEdited();
							AddSpacing(0);
							ImGui::Text("Developer Team");
							ImGui::InputText("Team Name", &teamName);
							if (!onWindow) onWindow = ImGui::IsItemEdited();
							AddSpacing(1);

							AddSeparator(2);

							AddSpacing(0);
							ImGui::Text("ENGINE PERFORMANCE");
							AddSpacing(1);
							ImGui::Text("Framerate");
							ImGui::SliderInt(" FPS", &app->fps, 5, 60, "FPS: %d");
							AddSpacing(0);

							char title[25];
							framerate = app->fpsLog[app->fpsLog.size() - 2];
							sprintf_s(title, 25, "Framerate %.1f", framerate);
							ImGui::Text("Framerate Graph");
							ImGui::PlotHistogram("##framerate", &app->fpsLog[0], app->fpsLog.size(), 0, title, 0.0f, 100.0f, ImVec2(307, 100));
							ImGui::SliderInt(" FBL", &app->frameBarLimit, 30, 80, "Bars: %d");

							AddSpacing(0);

							milliseconds = app->msLog[app->msLog.size() - 2];
							sprintf_s(title, 25, "Milliseconds %0.1f", milliseconds);
							ImGui::Text("Milliseconds Graph");
							ImGui::PlotHistogram("##milliseconds", &app->msLog[0], app->msLog.size(), 0, title, 0.0f, 40.0f, ImVec2(307, 100));
							ImGui::SliderInt(" MSG", &app->msBarLimit, 30, 80, "Bars: %d");

							AddSpacing(1);

							AddSeparator(2);
						}
					if (ImGui::CollapsingHeader("Window"))
					{
						Point prev = wSize;
						int prevProportion = wSizeProportion;
						float prevBright = brightLevel;

						AddSpacing(0);
						ImGui::Text("Brightness");
						ImGui::SliderFloat("B", &brightLevel, 0.0f, 1.0f, "Value: %.3f");
						if (prevBright != brightLevel) app->window->SetWinBrightness(brightLevel);

						AddSpacing(0);
						if (ImGui::Checkbox("Keep Proportion", &wKeepProportion)) wKeepProportion = wKeepProportion;

						if (!wKeepProportion)
						{
							AddSpacing(0);
							ImGui::Text("Width");
							ImGui::SliderInt("W", (int*)&wSize.x, 1, SCREEN_WIDTH, "%d");

							AddSpacing(0);
							ImGui::Text("Height");
							ImGui::SliderInt("H", (int*)&wSize.y, 1, SCREEN_HEIGHT, "%d");

							if (wSize != prev) app->window->SetWinSize((int)wSize.x, (int)wSize.y);
						}
						else
						{
							AddSpacing(0);
							ImGui::Text("Width / Height");
							ImGui::SliderInt("W", &wSizeProportion, 1, 100, "%d");

							if (wSizeProportion != prevProportion) app->window->SetWinSize(int(floor(SCREEN_WIDTH * wSizeProportion / 100)), int(floor(SCREEN_HEIGHT * wSizeProportion / 100)));
						}

						AddSpacing(1);
						if (ImGui::Checkbox(" VSync", &wVSync)) app->render->SetVSync(wVSync);

						AddSpacing(0);
						if (ImGui::Checkbox(" Full Screen    ", &wFullScreen)) app->window->SetWinFullScreen(wFullScreen);
						ImGui::SameLine();
						if (ImGui::Checkbox(" Resizable", &wResizable)) app->SaveRestartPropierties();

						AddSpacing(0);
						if (ImGui::Checkbox(" Borderless     ", &wBorderless)) app->window->SetWinBorders(wBorderless);
						ImGui::SameLine();
						if (ImGui::Checkbox(" Full Desktop", &wFullDesktop)) app->window->SetWinFullScreen(wFullDesktop);

						AddSpacing(1);
					}
					if (ImGui::CollapsingHeader("Input"))
					{
						AddSpacing(0);
						ImGui::Text("Mouse Position: %d %d", app->input->GetMouseX(), app->input->GetMouseY());
						AddSpacing(0);
						ImGui::Text("Mouse Speed: %d %d", app->input->GetMouseXMotion(), app->input->GetMouseYMotion());
						AddSpacing(0);
						ImGui::Text("Mouse Wheel: %d %d", app->input->GetMouseWheel(), app->input->GetMouseZH());
						AddSpacing(1);
					}
					if (ImGui::CollapsingHeader("Camera"))
					{
						AddSpacing(0);
						float* speed = app->camera->GetSpeed();
						ImGui::SliderFloat("Speed", speed, 0.1f, 1.0f);
						AddSpacing(0);
						float* sens = app->camera->GetSensitivity();
						ImGui::SliderFloat("Sensitivity", sens, 0.1f, 0.5f);
						AddSpacing(1);
					}
					if (ImGui::CollapsingHeader("Hardware"))
						{
							AddSpacing(0);
							SDL_version v;
							SDL_GetVersion(&v);
							ImGui::Text("SDL Version: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(YELLOW), "%d.%d.%d", v.major, v.minor, v.patch);
							AddSpacing(0);
							AddSeparator(2);
							AddSpacing(0);
							ImGui::Text("CPUs: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(YELLOW), "%d (Cache: %dkb)", SDL_GetCPUCount(), SDL_GetCPUCacheLineSize());
							AddSpacing(0);
							ImGui::Text("System RAM: "); ImGui::SameLine(); ImGui::TextColored(ImVec4(YELLOW), "%d", SDL_GetSystemRAM);
							AddSpacing(0);
							ImGui::Text("Caps: "); ImGui::SameLine();
							{
								if (SDL_Has3DNow()) ImGui::TextColored(ImVec4(YELLOW), "3DNow"); ImGui::SameLine();
								if (SDL_HasAltiVec()) ImGui::TextColored(ImVec4(YELLOW), "AltiVec"); ImGui::SameLine();
								if (SDL_HasAVX()) ImGui::TextColored(ImVec4(YELLOW), "AVX"); ImGui::SameLine();
								if (SDL_HasAVX2()) ImGui::TextColored(ImVec4(YELLOW), "AVX2"); ImGui::SameLine();
								if (SDL_HasMMX()) ImGui::TextColored(ImVec4(YELLOW), "MMX"); ImGui::SameLine();
								if (SDL_HasRDTSC()) ImGui::TextColored(ImVec4(YELLOW), "RDTSC");
								if (SDL_HasSSE()) ImGui::TextColored(ImVec4(YELLOW), "SSE"); ImGui::SameLine();
								if (SDL_HasSSE2()) ImGui::TextColored(ImVec4(YELLOW), "SSE2"); ImGui::SameLine();
								if (SDL_HasSSE3()) ImGui::TextColored(ImVec4(YELLOW), "SSE3"); ImGui::SameLine();
								if (SDL_HasSSE41()) ImGui::TextColored(ImVec4(YELLOW), "SSE41"); ImGui::SameLine();
								if (SDL_HasSSE42()) ImGui::TextColored(ImVec4(YELLOW), "SSE42");
							}
							AddSpacing(0);
							AddSeparator(2);
						}

					ImGui::EndTabItem();
				}
				if (ImGui::BeginTabItem("Geometry View"))
				{
					AddSpacing(0);
					ImGui::Text("VIEW SETTINGS");
					AddSpacing(1);

					if (ImGui::Checkbox("Depth Test", &depthTest))
						app->render->ToggleGeometryView(GeometryView::DEPTH_TEST, depthTest);

					AddSpacing(0);

					if (ImGui::Checkbox("Cull Face", &cullFace))
						app->render->ToggleGeometryView(GeometryView::CULL_FACE, cullFace);

					AddSpacing(0);

					if (ImGui::Checkbox("Lighting", &lighting))
						app->render->ToggleGeometryView(GeometryView::LIGHTING, lighting);

					AddSpacing(0);

					if (ImGui::Checkbox("Color Material", &colorMaterial))
						app->render->ToggleGeometryView(GeometryView::COLOR_MATERIAL, colorMaterial);

					AddSpacing(0);

					if (ImGui::Checkbox("Texture 2D", &texture2D))
						app->render->ToggleGeometryView(GeometryView::TEXTURE_2D, texture2D);

					ImGui::EndTabItem();
				}
			}
			ImGui::EndTabBar();
		}
		ImGui::End();
	}

	return open;
}

bool EditorScene::ShowHierarchyWindow(bool open)
{
	if (open)
	{
		if (ImGui::Begin("Hierarchy", &open))
		{
			if (!onWindow) onWindow = ImGui::IsWindowHovered();
			AddHelper("To create Shapes3Ds, right-click this window", "Help");
			
			if (ImGui::BeginPopupContextWindow("Create"))
			{
				ImGui::Text("Create:");
				if (ImGui::MenuItem("Empty Shape"))
					CreatePrimitive(EMPTY3D);

				if (ImGui::MenuItem("Empty Child"))
					CreatePrimitive(EMPTY3D, true);

				if (ImGui::BeginMenu("Primitives"))
				{
					if (ImGui::MenuItem("Cube"))
						CreatePrimitive(CUBE3D);

					if (ImGui::MenuItem("Line"))
						CreatePrimitive(LINE3D);

					if (ImGui::MenuItem("Pyramid"))
						CreatePrimitive(PYRAMID3D);

					if (ImGui::MenuItem("Cylinder"))
						CreatePrimitive(CYLINDER3D);

					if (ImGui::MenuItem("Sphere"))
						CreatePrimitive(SPHERE3D);

					if (ImGui::MenuItem("Plane"))
						CreatePrimitive(PLANE3D);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Test Models"))
				{
					if (ImGui::MenuItem("Cube"))
					{
						Model* m = new Model({ 0, 0, 0 }, 1.0f);
						m->LoadModel("Assets/Models/cube.fbx");
						PushShape3D(m);
					}
					if (ImGui::MenuItem("Warrior"))
					{
						Model* m = new Model({ 0, 0, 0 }, 1.0f);
						m->LoadModel("Assets/Models/warrior.FBX");
						PushShape3D(m);
					}
					if (ImGui::MenuItem("Baker House"))
					{
						Model* m = new Model({ 0, 0, 0 }, 1.0f);
						m->LoadModel("Assets/Models/BakerHouse.fbx", "Assets/Textures/baker_house_texture.png");
						PushShape3D(m);
					}
					ImGui::EndMenu();
				}

				ImGui::EndPopup();
			}

			AddSeparator(2);
			AddSpacing(1);
			selectId = 0;
			int nullId = -2;

			TravelShapes(shapes);
			bool test = false;
			ImGui::Selectable("", &test, ImGuiSelectableFlags_Disabled, ImVec2{ImGui::GetItemRectMax()});

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				ImGui::SetDragDropPayload("DragDropHierarchy", &nullId, sizeof(int), ImGuiCond_Once);
				ImGui::EndDragDropSource();
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
				{
					IM_ASSERT(payload->DataSize == sizeof(Shape3D*));
					int droppedId = *(const int*)payload->Data;
					int index = -1;
					Shape3D* drop = GetShapeFromId(*shapes, droppedId, &index);
					if (drop && drop->hasParent && index != -1)
					{
						std::vector<Shape3D*>* v = &drop->parent->childs;
						v->erase(v->begin() + index);
						drop->hasParent = false;
						drop->parent = nullptr;
						shapes->push_back(drop);
						SetValidId(*shapes);
					}
				}
				ImGui::EndDragDropTarget();
			}
		}
		ImGui::End();
	}

	return open;
}

bool EditorScene::ShowInspectorWindow(bool open)
{
	if (open)
	{
		if (ImGui::Begin("Inspector", &open))
		{
			if (!onWindow) onWindow = ImGui::IsWindowHovered();

			Shape3D* s = prevShape;

			if (selectId != 0)
			{
				if (selectId != prevSelectId) s = GetShapeFromId(*shapes, selectId);
				prevSelectId = selectId;
				prevShape = s;

				AddSeparator(2);
				AddSpacing(0);

				// NAME
				char buffer[24] = {};
				sprintf_s(buffer, "Name: %s %d", s->GetName(), selectId);
				ImGui::Text(buffer);
				AddSpacing(1);

				// Type
				ImGui::Text("Type: %s", s->WriteShapeType().c_str());
				AddSpacing(0);

				switch (s->GetShapeType())
				{
				case PYRAMID3D:
				{
					Pyramid3D* py = (Pyramid3D*)s;
					ImGui::Text("    Height: %.2f", py->GetHeight());
					break;
				}
				case CYLINDER3D:
				{
					Cylinder3D* cy = (Cylinder3D*)s;
					ImGui::Text("    Height: %.2f", cy->GetHeight());
					ImGui::Text("    Radius: %.2f", cy->GetRadius());
					ImGui::Text("    Segments: %d", cy->GetSegments());
					break;
				}
				case PLANE3D:
				{
					Plane3D* p = (Plane3D*)s;
					ImGui::Text("    Normal: {%d, %d, %d}", (int)p->GetNormal().x, (int)p->GetNormal().y, (int)p->GetNormal().z);
					break;
				}
				case SPHERE3D:
				{
					Sphere3D* sp = (Sphere3D*)s;
					/*subdivision = sub
						interleavedStride*/
					ImGui::Text("    Radius: %.1f", sp->GetRadius());
					ImGui::Text("    Subdivisions: %.1f", sp->GetSubdivision());
					break;
				}
				}

				// TRANSFORM
				if(ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
				{
					// we create new Point3D so that the transforms of the meshes are modificable
					Point3D position = s->GetPosition();
					Rotation rotation = s->GetRotation();
					Point3D scale = s->GetScale();
					Point3D rot = rotation.ToEulerAngles();

					if (ImGui::DragFloat3("Position", (float*)&position, 0.25))
					{
						s->SetPosition(position);
					}

					if (ImGui::DragFloat3("Scale", (float*)&scale, 0.25))
					{
						s->SetScale(scale);
					}

					if (ImGui::DragFloat3("Rotation", (float*)&rot, 0.25))
					{
						rotation.FromEulerAngles(rot);
						s->SetRotation(rotation);
					}
				}
				AddSpacing(0);

				AddSpacing(1);
				AddSeparator(1);
				AddSpacing(1);

				// MESH
				ImGui::Text(" Mesh:");
				AddSpacing(0);

				if (s->GetShapeType() != MODEL3D)
				{
					ImGui::BulletText("Num: %d", 1);
					AddSpacing(1);
				}
				else
				{
					Model* m = (Model*)s;
					// Num Of Mesh
					ImGui::BulletText("Num: %d", m->meshes.size());
					AddSpacing(1);

					// Model Path
					ImGui::BulletText("Model path: %s", m->filePath.c_str());
					AddSpacing(1);
				}

				// Solid
				ImGui::BulletText("Solid: "); ImGui::SameLine(); s->solid ? ImGui::Text("true") : ImGui::Text("false");
				AddSpacing(1);

				// Edges
				ImGui::BulletText("Edges: "); ImGui::SameLine(); s->edges ? ImGui::Text("true") : ImGui::Text("false");
				AddSpacing(1);

				if (s->GetShapeType() == MODEL3D)
				{
					// Normals
					ImGui::BulletText("Normals: "); ImGui::SameLine(); s->normals ? ImGui::Text("true") : ImGui::Text("false");
					AddSpacing(1);
				}
				AddSeparator(1);
				AddSpacing(1);

				// TEXTURE
				ImGui::Text(" Texture:");
				
				AddSpacing(0);

				if (s->GetShapeType() != MODEL3D)
				{
					ImGui::BulletText("No Texture");
					AddSpacing(1);
				}
				else
				{
					Model* m = (Model*)s;

					//ImGui::Image((void*)(intptr_t)myImageTexture, ImVec2(imageWidth, imageHeight));

					// Size
					ImGui::BulletText("Size: ");
					ImGui::Text("    Width: %.2f", m->GetSize().x);
					ImGui::Text("    Height: %.2f", m->GetSize().y);
					AddSpacing(1);

					// Texture Path
					ImGui::BulletText("Texture path: %s", m->texturePath.c_str());
					AddSpacing(1);

					// Show texture
					ImGui::Checkbox("Texture", &m->showTexture);
					AddSpacing(1);
				}
			}
		}

		ImGui::End();
	}
	return open;
}

bool EditorScene::ShowAssetsWindow(bool open)
{
	if (open)
	{
		if (ImGui::Begin("Assets", &open))
		{
			if (!onWindow) onWindow = ImGui::IsWindowHovered();

			if (ImGui::Button("Refresh directory"))
				assets->ParseFiles();

			ImGui::SameLine();

			if (ImGui::Button("Back"))
				assets->ParseBackwardFiles();

			ImGui::SameLine(0.0f, 600);
			ImGui::PushItemWidth(150);
			ImGui::SliderInt("Size", &assetsWidth, 0, 28, "%d");
			ImGui::PopItemWidth();

			// Number of assets in total
			int assetSize = assets->GetAssetsNumber();

			// Number of assets per table
			int numMaxAssets = 8;
			if (assetsWidth >= 11) numMaxAssets = 7;

			// Number of tables to make
			int numRows = 1; // Number of tables to make
			if (assetSize > numMaxAssets) numRows = ceil((float)assetSize / numMaxAssets);

			AddSpacing(1);
			AddSeparator(1);

			// Assets via Buttons
			for (int a = 0; a < numRows; a++)
			{
				if (assetSize == 0) break;
				AddSpacing(1);

				int columns = 0;
				if (numRows == 1) columns = assetSize;
				else
				{
					if (a == numRows - 1) columns = assetSize - (numMaxAssets * (numRows - 1));
					else
						columns = numMaxAssets;
				}

				for (int i = 0; i < columns; i++)
				{
					char n[20];

					if (columns > 1)
					{
						sprintf_s(n, "col%d", a);
						ImGui::Columns(columns, n, false);
						ImGui::SetColumnWidth(i, (assetsWidth + 120));
					}

					ImGui::Text(assets->GetAssetAt(i + (columns * a))->name.c_str());
					ImGui::PushID(i);

					ImageTexture tex = ImageTexture::IMG_FOLDER;
					switch (assets->GetAssetAt(i)->type)
					{
					case AssetType::DIRECTORY: tex = ImageTexture::IMG_FOLDER; break;
					case AssetType::FILE: tex = ImageTexture::IMG_FILE; break;
					}

					if (AddImageButton(import->GetImage(tex), (int)10))
					{
						switch (assets->GetAssetAt(i)->type)
						{
						case AssetType::DIRECTORY:
						{
							assets->ParseForwardFiles(assets->GetAssetAt(i + (columns * a))->name.c_str());
							break;
						}
						case AssetType::FILE: 
						{
							string format = assets->GetAssetAt(i)->name;
							int fSize = format.size() - 4;
							for (unsigned int i = 0; i < fSize; i++) format.erase(format.begin());
							format.shrink_to_fit();
							string FBX = ".FBX";
							string fbx = ".fbx";

							if (SameString(format, FBX) || SameString(format, fbx))
							{
								Model* m = new Model({0, 0, 0});
								m->ImportModel(assets->GetAssetAt(i)->name.c_str());
								PushShape3D(m);
							}
							else
							{
								Shape3D* s = GetShapeFromId(*shapes, selectId);
								if (s->GetShapeType() == ShapeType::MODEL3D)
								{
									Model* m = (Model*)s;
									string path = "Assets/Textures/";
									path += assets->GetAssetAt(i)->name;
									m->LoadTexture(path.c_str());
								}
							}
							break;
						}
						}
					}
					
					ImGui::PopID();
					ImGui::NextColumn();
				}
			}
		}
		ImGui::End();
	}
	return open;
}

bool EditorScene::ShowGameStateWindow(bool open)
{
	if (open)
	{
		if (ImGui::Begin("Game State", &open))
		{
			if (!onWindow) onWindow = ImGui::IsWindowHovered();
			ImGui::Dummy(ImVec2{ ImGui::GetWindowSize().x/2-120,0 });
			ImGui::SameLine();
			if (ImGui::Button("Play"))
				app->scene->GameTime.Start();
			ImGui::SameLine();
			if (ImGui::Button("Stop"))
				app->scene->GameTime.Stop();
			ImGui::SameLine();
			if (ImGui::Button("Resume"))
				app->scene->GameTime.Resume();
			ImGui::SameLine();
			if (ImGui::Button("Pause"))
				app->scene->GameTime.Pause();
		}
		ImGui::End();
	}

	return open;
}

bool EditorScene::ShortCuts()
{
	bool ret = true;

	bool shift = (app->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_RSHIFT) == KEY_REPEAT);
	bool ctrl = (app->input->GetKey(SDL_SCANCODE_LCTRL) == KEY_REPEAT || app->input->GetKey(SDL_SCANCODE_RCTRL) == KEY_REPEAT);

	if (app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) ret = false;

	if (!shift && !ctrl) // NO CLICK
	{
		if (app->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) DeleteShape();
	}
	else if (shift && !ctrl) // SHIFT
	{
		if (app->input->GetKey(SDL_SCANCODE_DELETE) == KEY_DOWN) DeleteAllShapes();
	}
	else if (!shift && ctrl) // CTRL
	{
		if (app->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN) app->Save("NNE_Project_Saving", FileContent::PROJECT);
		if (app->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		{
			DeleteAllShapes(false);
			app->Load("NNE_Project_Saving", FileContent::PROJECT);
			SetValidId(*shapes);
		}
		if (app->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN) DuplicateSelectedShape();
		if (app->input->GetKey(SDL_SCANCODE_T) == KEY_DOWN) app->scene->SetGeometryView(GeometryView::TEXTURE_2D, !app->scene->GetGeometryView(GeometryView::TEXTURE_2D));
	}

	return ret;
}

// -------------------------------------------------

void EditorScene::CreatePrimitive(ShapeType sT, bool asChild)
{
	Shape3D* selected = nullptr;
	vector<Shape3D*>* vec = nullptr;
	if (asChild)
	{
		selected = GetShapeFromId(*shapes, selectId);
		if (selected->id == 0 && selected->parent == nullptr)
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_INFORMATION,
				"Shapes Error",
				"\nAny shape selected",
				app->window->mainWindow
			);
			return;
		}

		vec = &selected->childs;
	}

	Shape3D* push = nullptr;

	switch (sT)
	{
	case CUBE3D: PushShape3D(new Cube3D(), vec); break;
	case LINE3D: PushShape3D(new Line3D(), vec); break;
	case PYRAMID3D: PushShape3D(new Pyramid3D(), vec); break;
	case CYLINDER3D: PushShape3D(new Cylinder3D(), vec); break;
	case PLANE3D: PushShape3D(new Plane3D(), vec); break;
	case SPHERE3D: PushShape3D(new Sphere3D(), vec); break;
	case EMPTY3D: PushShape3D(new Empty3D(), vec); break;
	}

	if (asChild)
	{
		vec->at(vec->size() - 1)->hasParent = true;
		vec->at(vec->size() - 1)->parent = selected;
	}
}

void EditorScene::PushShape3D(Shape3D* s3D, vector<Shape3D*>* vector)
{
	if (vector == nullptr) vector = shapes;

	s3D->selected = false;
	vector->push_back(s3D);
	SetValidId(*shapes);
}

void EditorScene::DuplicateSelectedShape()
{
	int size = shapes->size();
	Shape3D* select = GetShapeFromId(*shapes, selectId);

	if (select != nullptr && select->selected)
	{
		switch (select->GetShapeType())
		{
		case CUBE3D: PushShape3D(new Cube3D(*(Cube3D*)select)); break;
		case LINE3D: PushShape3D(new Line3D(*(Line3D*)select)); break;
		case PYRAMID3D: PushShape3D(new Pyramid3D(*(Pyramid3D*)select)); break;
		case CYLINDER3D: PushShape3D(new Cylinder3D(*(Cylinder3D*)select)); break;
		case PLANE3D: PushShape3D(new Plane3D(*(Plane3D*)select)); break;
		case SPHERE3D: PushShape3D(new Sphere3D(*(Sphere3D*)select)); break;
		case MODEL3D: PushShape3D(new Model(*(Model*)select)); break;
		case EMPTY3D: PushShape3D(new Empty3D(*(Empty3D*)select)); break;
		}
	}
}

void EditorScene::DeleteShape()
{
	Shape3D* erase = GetShapeFromId(*shapes, selectId);
	if (erase != nullptr && erase->selected)
	{
		DeleteShapeFromId(shapes, selectId);
		erase = nullptr;
		SetValidId(*shapes);

	}
	else
	{
		SDL_ShowSimpleMessageBox(
			SDL_MESSAGEBOX_INFORMATION,
			"Shapes Error",
			"\nAny shape selected",
			app->window->mainWindow
		);
	}
}

void EditorScene::DeleteAllShapes(bool enableMessage)
{
	if (shapes->size() > 1)
	{
		DeleteAll(shapes, 1);
		SetValidId(*shapes);
	}
	else
	{
		if (enableMessage)
		{
			SDL_ShowSimpleMessageBox(
				SDL_MESSAGEBOX_INFORMATION,
				"Shapes Error",
				"\nNo shape left in the scene",
				app->window->mainWindow
			);
		}
	}
}

void EditorScene::EditTransform(bool editTransformDecomposition)
{
	static ImGuizmo::OPERATION currentOperation(ImGuizmo::TRANSLATE);
	static ImGuizmo::MODE currentMode(ImGuizmo::LOCAL);
	static bool useSnap = false;
	static float snap[3] = { 1.f, 1.f, 1.f };
	static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
	static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
	static bool boundSizing = false;
	static bool boundSizingSnap = false;

	if (editTransformDecomposition)
	{
		if (app->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN)
			currentOperation = ImGuizmo::TRANSLATE;
		if (app->input->GetKey(SDL_SCANCODE_E) == KEY_DOWN)
			currentOperation = ImGuizmo::ROTATE;
		if (app->input->GetKey(SDL_SCANCODE_R) == KEY_DOWN)
			currentOperation = ImGuizmo::SCALE;
	}
}

void EditorScene::TravelShapes(vector<Shape3D*>* shapeVect, int depth)
{
	int startingPoint = 0;
	if (depth == 0) startingPoint = 1;
	for (unsigned int i = startingPoint; i < shapeVect->size(); i++)
	{
		ImGui::Dummy({ float(depth * 10), 0.0f });
		ImGui::SameLine();
		Shape3D* s = shapeVect->at(i);
		char buffer[24] = {};
		if (depth == 0) sprintf_s(buffer, "%s %d", s->GetName(), i);
		else
			sprintf_s(buffer, "%s %d.%d", s->GetName(), i, depth);

		ImGui::PushID(i);
		ImGui::Checkbox("", &s->draw);
		ImGui::SameLine();
		ImGui::PopID();

		if (ImGui::Selectable(buffer, &s->selected))
		{
			DiselectShapes(*shapes, s);
			float shape = (float)s->selected;
			ImGuizmo::SetOrthographic(false);
			ImGuizmo::SetDrawlist();
			ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, (float)ImGui::GetWindowWidth(), (float)ImGui::GetWindowHeight());
			ImGuizmo::Manipulate(app->camera->GetViewMatrix(), app->camera->GetViewMatrix(), ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, &shape);
		}

		if (ImGui::BeginDragDropSource()) 
		{
			ImGui::SetDragDropPayload("DragDropHierarchy", &s->id, sizeof(int), ImGuiCond_Once);
			ImGui::Text(buffer);
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			bool dropped = false;
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DragDropHierarchy"))
			{
				IM_ASSERT(payload->DataSize == sizeof(Shape3D*));
				int droppedId = *(const int*)payload->Data;
				int index = -1;
				Shape3D* drop = GetShapeFromId(*shapes, droppedId, &index);
				if (drop && index != -1)
				{
					std::vector<Shape3D*>* v = shapes;
					if (drop->hasParent)
					{
						v = &drop->parent->childs;
					}

					v->erase(v->begin() + index);
					drop->hasParent = true;
					drop->parent = s;
					s->childs.push_back(drop);
					SetValidId(*shapes);
					dropped = true;
				}
			}
			ImGui::EndDragDropTarget();
			if (dropped) return;
		}

		if (selectId == 0 && s->selected) selectId = s->id;

		if (!s->childs.empty()) TravelShapes(&s->childs, (depth + 1));

		AddSpacing(0);
	}
}

void EditorScene::DiselectShapes(vector<Shape3D*> shapes, Shape3D* ref)
{
	for (unsigned int a = 0; a < shapes.size(); a++)
	{
		if (shapes[a] != ref) shapes[a]->selected = false;
		if (!shapes[a]->childs.empty())
		{
			DiselectShapes(shapes[a]->childs, ref);
		}
	}
}

int EditorScene::SetValidId(vector<Shape3D*> shapes, int size)
{
	int s = size;
	for (unsigned int a = 0; a < shapes.size(); a++)
	{
		shapes[a]->id = s;
		s++;
		if (!shapes[a]->childs.empty()) s = SetValidId(shapes[a]->childs, s);
	}

	prevSelectId = -1;
	selectId = 0;
	prevShape = nullptr;

	return s;
}
