#ifndef __AUDIO_SOURCE_COMPONENT_H__
#define __AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"
#include "AudioSystem.h"

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
	}

	void Update(Shape3D* afected)
	{
		if (gameTimer->GetTimerState() != RUNNING) return;

		//audio->PlayAudio(track.source);
	}

	void Draw(bool* onWindow = nullptr)
	{
		if (ImGui::Button("Browse Mono Wav"))
		{
			if (!BrowseAudio(false, false))
			{
				// Malament
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Browse Stereo Wav"))
		{
			if (!BrowseAudio(true, false))
			{
				// Malament
			}
		}

		if (ImGui::Button("Browse Mono Mp3"))
		{
			if (!BrowseAudio(false, true))
			{
				// Malament
			}
		}
		ImGui::SameLine();
		if (ImGui::Button("Browse Stereo Mp3"))
		{
			if (!BrowseAudio(true, true))
			{
				// Malament
			}
		}

		// Debug (Should be in the update)
		if (play) play = audio->PlayAudio(track.source);

		if (track.channels != 0)
		{
			if (ImGui::Button("Play Test"))
			{
				play = true;
			}
			ImGui::SameLine();
			if (ImGui::Button("Edit")) open = !open;
		}

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
			bool mono = track.channels == 1;

			ImGui::Text("%s", track.name.c_str());
			ImGui::Spacing();
			ImGui::Text("%d kHz | %d-Bits", track.sampleRate, track.bits);
			ImGui::Spacing();
			ImGui::Text("%d Channels", track.channels);
			if (!mono) 
			{
				ImGui::SameLine(); ImGui::Text(" (No Pan)");
			}

			ImGui::Dummy(ImVec2{ 0.0f, 4.2f });

			ImGui::Dummy(ImVec2{ 3.8f, 0.0f }); ImGui::SameLine();
			ImGui::VSliderFloat("-0", ImVec2{15, 155}, &volume, 0.0f, 100.0f, "");
			if (ImGui::IsItemDeactivatedAfterEdit()) SetVolume(volume);

			ImGui::SameLine(0.0f, 20.0f);
			if (ImGui::BeginTable("Column Table", 2))
			{
				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
				ImGui::Dummy(ImVec2{0.0f, 6.0f});

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
				ImGui::Dummy(ImVec2{ 4.5f, 0.0f }); ImGui::SameLine();
				if (ImGui::Knob("L / R", &pan, -0.5, 0.5, false, mono, 2.5f, &knobReminder2) && mono) SetPanning(pan);
				ImGui::Dummy(ImVec2{ 0.0f, 10.0f });

				ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
				if (ImGui::Knob("Transpose", &transpose, -24.0, 24.0, false, true, -10, &knobReminder1)) SetTranspose(transpose);
				
			}
			ImGui::EndTable();

		}
		ImGui::End();
	}

	bool BrowseAudio(bool stereo, bool mp3)
	{
		if (mp3)
		{
			if (stereo) track = audio->LoadAudio("Assets/Audio/SplitDuty_FinalBoss_Soundtrack.mp3");
			else
				track = audio->LoadAudio("Assets/Audio/SplitDuty_FinalBoss_Mono_Soundtrack.mp3");
		}
		else
		{
			if (stereo) track = audio->LoadAudio("Assets/Audio/SplitDuty_Meh_Fx.wav");
			else
				track = audio->LoadAudio("Assets/Audio/bounce.wav");
		}
		
		track.source = audio->CreateAudioSource(track.buffer, true);

		SetVolume(volume);
		SetPanning(pan);
		SetTranspose(transpose);

		return true;
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

private: // Variables

	float volume = 100.0f;
	float pan = 0;
	float transpose = 0;
	bool knobReminder1 = false;
	bool knobReminder2 = false;
	bool play = false;
	Track track;

	AudioSystem* audio = nullptr;
};

#endif // !__AUDIO_SOURCE_COMPONENT_H__
