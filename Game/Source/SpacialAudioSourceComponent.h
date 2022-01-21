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
		if (playOnStart) audio->PlayAudio(track.source);
	}

	void Update(Shape3D* afected)
	{
		if (gameTimer->GetTimerState() != RUNNING) return;

		track.source = audio->CreateAudioSource(track.buffer, true);
	}

	void Draw(bool* onWindow = nullptr)
	{
		if (ImGui::Button("Browse Audio")) browsing = true;

		if (track.channels != 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("Edit"))
			{
				open = !open;
			}

			ImGui::Spacing();
			ImGui::Text("Options");
			ImGui::Checkbox("Play on start", &playOnStart);
		}


		if (ImGui::Button("Play Test"))
		{
			audio->PlayAudio(track.source);
		}

		if (ImGui::Button("Edit")) open = !open;

		if (open) DrawWindow(onWindow);

		if (browsing) browsing = BrowseAudio();
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
		bool ret = true;

		// open Dialog Simple
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose an Audio File", ".wav,.mp3,.flac", ".");

		//display
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey"), ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove)
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk() == true)
			{
				std::string path = ImGuiFileDialog::Instance()->GetCurrentPath() + "\\" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				track = audio->LoadAudio(path.c_str());
				track.source = audio->CreateAudioSource(track.buffer, true);
				/*SetVolume(volume);
				SetPanning(pan);
				SetTranspose(transpose);*/
				ret = false;
			}
		}

		return ret;
	}

	void SetVolume(float volume)
	{
		if (mute)
		{
			alSourcef(track.source, AL_GAIN, 0);
			return;
		}

		volume = Pow(volume, 2.5f) / 1000.0f;
		if (volume > 99.0f) volume = 100.0f;
		alSourcef(track.source, AL_GAIN, volume / 100);
	}

	void SetPanning(float pan)
	{
		pan = pan * -1;
		alSource3f(track.source, AL_POSITION, pan, 0, -sqrtf(1.0f - pan * pan));
	}

	void SetTranspose(float transpose)
	{
		transpose = exp(0.0577623f * transpose);
		if (transpose > 4.0f) transpose = 4.0f;
		if (transpose < 0.25f) transpose = 0.25f;
		if (transpose > 0.98f && transpose < 1.2f) transpose = 1.0f;
		alSourcef(track.source, AL_PITCH, transpose);
	}

private: // Variables

	int volume = 100;
	float pan = 0;
	float transpose = 0;
	bool browsing = false;
	bool mute = false;
	bool playOnStart = true;
	Track track;

	AudioSystem* audio = nullptr;
};

#endif // !__SPACIAL_AUDIO_SOURCE_COMPONENT_H__

