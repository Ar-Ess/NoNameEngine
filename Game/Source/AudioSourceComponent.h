#ifndef __AUDIO_SOURCE_COMPONENT_H__
#define __AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"

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

			ImGui::Dummy(ImVec2{ 3.8f, 0.0f }); ImGui::SameLine();
			ImGui::VSliderInt("- 0", ImVec2{15, 150}, &volume, -128.0f, 0.0f, "");
			ImGui::Dummy(ImVec2{ 0.0f, 3.0f });
			ImGui::Knob("L / R", &pan, -50.0, 50.0, false);
		}
		ImGui::End();
	}

private: // Variables

	int volume = 0;
	float pan = 0;
};

#endif // !__AUDIO_SOURCE_COMPONENT_H__
