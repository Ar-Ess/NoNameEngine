#ifndef __SPACIAL_AUDIO_SOURCE_COMPONENT_H__
#define __SPACIAL_AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"
#include "AudioSystem.h"

class SpacialAudioSourceComponent : public Component
{
public:
	SpacialAudioSourceComponent(Timer* timer, AudioSystem* audioSystem) : Component("Spacial Audio Source", ComponentID::SPACIAL_AUDIO_SOURCE_COMPONENT)
	{
		gameTimer = timer;
		audio = audioSystem;
	}
	~SpacialAudioSourceComponent() {}

	void Start(Shape3D* afected)
	{

	}

	void Update(Shape3D* afected)
	{
		if (gameTimer->GetTimerState() != RUNNING) return;
	}

	void Draw(bool* onWindow = nullptr)
	{
		if (ImGui::Button("Browse..."))
		{
			if (!BrowseAudio())
			{
				// Malament
			}
		}

		if (ImGui::Button("Play Test"))
		{
			audio->PlayAudio(track.source);
		}

		if (ImGui::Button("Edit")) open = !open;

		if (open) DrawWindow(onWindow);
	}

	void End(Shape3D* afected)
	{

	}

private: // Methods

	void DrawWindow(bool* onWindow)
	{
		if (ImGui::Begin(title.c_str(), &open))
		{
			if (onWindow != nullptr && !*onWindow) *onWindow = ImGui::IsWindowHovered();

			ImGui::Text("Name of the sample");
			ImGui::Text("kHz, Bit, Channels info");

			ImGui::Dummy(ImVec2{ 0.0f, 3.8f });

			ImGui::Dummy(ImVec2{ 3.8f, 0.0f }); ImGui::SameLine();
			ImGui::VSliderInt("-0", ImVec2{ 15, 155 }, &volume, -128.0f, 0.0f, "");
			ImGui::SameLine(0.0f, 20.0f);
			if (ImGui::BeginTable("Column Table", 2))
			{
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
				ImGui::Dummy(ImVec2{ 0.0f, 6.0f });

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
				ImGui::Dummy(ImVec2{ 4.5f, 0.0f }); ImGui::SameLine();
				ImGui::Knob("L / R", &pan, -50.0, 50.0, false, 2.5f);
				ImGui::Dummy(ImVec2{ 0.0f, 10.0f });

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
				ImGui::Knob("Transpose", &transpose, -24.0, 24.0, false, -10);

			}
			ImGui::EndTable();

		}
		ImGui::End();
	}

	bool BrowseAudio()
	{
		track = audio->LoadAudio("Assets/Audio/SplitDuty_Meh_Fx.wav");
		track.source = audio->CreateAudioSource(track.buffer, true);

		return true;
	}

private: // Variables

	int volume = 0;
	float pan = 0;
	float transpose = 0;
	Track track;

	AudioSystem* audio = nullptr;
};

#endif // !__SPACIAL_AUDIO_SOURCE_COMPONENT_H__

