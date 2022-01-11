#ifndef __AUDIO_SOURCE_COMPONENT_H__
#define __AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"
#include "External/Wwise/include/AK/IBytes.h"
#include "External/Wwise/include/AK/SoundEngine/Common/AkSoundEngine.h"

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent() : Component("Audio Source", ComponentID::AUDIO_SOURCE_COMPONENT)
	{}
	~AudioSourceComponent() {}

	void Update(bool* onWindow = nullptr)
	{
		if (ImGui::Button("Edit")) open = !open;

		if (open) UpdateWindow(onWindow);
	}

private: // Methods

	void UpdateWindow(bool* onWindow)
	{
		if (ImGui::Begin(title.c_str(), &open))
		{
			if (onWindow != nullptr && !*onWindow) *onWindow = ImGui::IsWindowHovered();

			ImGui::Text("Name of the sample");
			ImGui::Text("kHz, Bit, Channels info");

			ImGui::Dummy(ImVec2{ 0.0f, 3.8f });

			ImGui::Dummy(ImVec2{ 3.8f, 0.0f }); ImGui::SameLine();
			ImGui::VSliderInt("-0", ImVec2{15, 155}, &volume, -128.0f, 0.0f, ""); 
			ImGui::SameLine(0.0f, 20.0f);
			if (ImGui::BeginTable("Column Table", 2))
			{
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
				ImGui::Dummy(ImVec2{0.0f, 6.0f});

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

private: // Variables

	int volume = 0;
	float pan = 0;
	float transpose = 0;
};

#endif // !__AUDIO_SOURCE_COMPONENT_H__
