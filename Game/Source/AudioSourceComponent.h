#ifndef __AUDIO_SOURCE_COMPONENT_H__
#define __AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"
#include "AudioSystem.h"
#include "External/ImGuiFileDialog/ImGuiFileDialog.h"

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent(Timer* timer, AudioSystem* audioSystem) : Component("Audio Source", ComponentID::AUDIO_SOURCE_COMPONENT)
	{
		gameTimer = timer;
		audio = audioSystem;
	}
	~AudioSourceComponent() {}

	void Start(Shape3D* afected)
	{
		knobReminder1 = false;
		knobReminder2 = false;

	}

	void Update(Shape3D* afected)
	{
		if (gameTimer->GetTimerState() != RUNNING) return;

		//audio->PlayAudio(track.source);
	}

	void Draw(bool* onWindow = nullptr)
	{
		if (ImGui::Button("Browse Audio")) browsing = true;

		if (track.channels != 0)
		{
			if (ImGui::Button("Edit"))
			{
				open = !open;
				offset = 0;
				knobReminder1 = false;
				knobReminder2 = false;
			}
		}

		UpdatePlayState();

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
			bool mono = track.channels == 1;

			if (ImGui::BeginTable("Upper Table", 2))
			{
				float winWidth = ImGui::GetWindowWidth();
				float resultWidth = winWidth > 627 ? 350.0f : (350.0f - (627.0f - winWidth));
				resultWidth = winWidth < 408 ? 148 : resultWidth;
				float width = winWidth > 627 ? ((winWidth * 0.545f) - 350) : ((winWidth * ((54.4f + ((627.0f - winWidth) / 100)) / 100)) - resultWidth);

				ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, resultWidth); // Default to 100.0f
				ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				{
					ImGui::Text("%s", track.name.c_str());
					ImGui::Spacing();
					ImGui::Text("%d kHz | %d-Bits", track.sampleRate, track.bits);
					ImGui::Spacing();
					ImGui::Text("%d Channels", track.channels);
					if (!mono)
					{
						ImGui::SameLine(); ImGui::Text(" (No Pan)");
					}
				}

				ImGui::TableSetColumnIndex(1);
				{
					char t1[] = { "Play" };
					char t2[] = { "Stop" };
					char* t = play ? t = t2 : t = t1;
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Dummy(ImVec2{ width, 0.0f }); ImGui::SameLine();
					if (ImGui::Button(t))
					{
						float time = track.duration * offset;
						play ? audio->StopAudio(track.source) : audio->PlayAudio(track.source, time);
					}
				}
			}
			ImGui::EndTable();

			ImGui::Dummy(ImVec2{ 0.0f, 4.2f });

			if (ImGui::BeginTable("Group Table", 2))
			{
				ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, 180.0f); // Default to 100.0f
				ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				{
					// SLIDERS & KNOBS
					ImGui::Dummy(ImVec2{ 0.0f, 35.0f });
					ImGui::Dummy(ImVec2{ 3.8f, 0.0f }); ImGui::SameLine();
					ImGui::VSliderFloat("-0", ImVec2{ 15, 155 }, &volume, 0.0f, 100.0f, "");
					if (ImGui::IsItemDeactivatedAfterEdit()) SetVolume(volume);
					ImGui::SameLine(0.0f, 20.0f);
					if (ImGui::BeginTable("Column Table", 2))
					{
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
						ImGui::Dummy(ImVec2{ 0.0f, 6.0f });

						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
						ImGui::Dummy(ImVec2{ 4.5f, 0.0f }); ImGui::SameLine();
						if (ImGui::Knob("L / R", &pan, -0.5, 0.5, false, mono, 2.5f, &knobReminder2) && mono) SetPanning(pan);
						ImGui::Dummy(ImVec2{ 0.0f, 10.0f });

						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
						if (ImGui::Knob("Transpose", &transpose, -24.0, 24.0, false, true, -10, &knobReminder1)) SetTranspose(transpose);
					}
					ImGui::EndTable();
				}

				ImGui::TableSetColumnIndex(1);
				{
					// VOLUME GRAPHIC
					float a[10] = {10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f};
					float width = ImGui::GetWindowWidth();
					ImGui::PushItemWidth(width - 220);
					ImGui::SliderFloat("##offset", &offset, 0.0f, 1.0f, "");
					ImGui::PopItemWidth();
					ImGui::PlotHistogram("##volumegraph", a, 10, 100.0f, "", 0.0f, 10.0f, ImVec2(width - 220, 100));
					ImGui::PlotHistogram("##volumegraph", a, 10, 0.0f, "", 10.0f, 0.0f, ImVec2(width - 220, 100), 4, true);
				}

				ImGui::EndTable();
			}
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
				SetVolume(volume);
				SetPanning(pan);
				SetTranspose(transpose);
				ret = false;
			}
		}

		return ret;
	}

	void SetVolume(float volume)
	{
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

	void UpdatePlayState()
	{
		ALint sourceState;
		alGetSourcei(track.source, AL_SOURCE_STATE, &sourceState);
		(sourceState == AL_PLAYING) ? play = true : play = false;
	}

private: // Variables

	float volume = 100.0f;
	float pan = 0;
	float transpose = 0;
	float offset = 0;
	bool knobReminder1 = false;
	bool knobReminder2 = false;
	bool play = false;
	bool browsing = false;
	Track track;

	AudioSystem* audio = nullptr;
};

#endif // !__AUDIO_SOURCE_COMPONENT_H__
