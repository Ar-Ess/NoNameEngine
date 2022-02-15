#ifndef __AUDIO_SOURCE_COMPONENT_H__
#define __AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"
#include "Effects.h"
#include "External/ImGuiFileDialog/ImGuiFileDialog.h"

class AudioSourceComponent : public Component
{
public:
	AudioSourceComponent(Timer* timer, AudioSystem* audioSystem) : Component("Audio Source", ComponentID::AUDIO_SOURCE_COMPONENT)
	{
		gameTimer = timer;
		audio = audioSystem;

		SetVolume(volume);
		SetPanning(pan);
		SetTranspose(transpose);
		SetLoop(loop);
	}
	~AudioSourceComponent() 
	{
		fxTracker.clear();
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			effects[i]->Disconnect(track.source);
			delete effects[i];
		}
		effects.clear();
		this->title.clear();
	}

	void Start(Shape3D* afected)
	{
		knobReminder1 = false;
		knobReminder2 = false;
		if (playOnStart) audio->PlayAudio(track.source);
	}

	void Update(Shape3D* afected)
	{
		if (gameTimer->GetTimerState() != RUNNING) return;

		//audio->PlayAudio(track.source);
	}

	void Draw(Shape3D* affected, bool* onWindow = nullptr)
	{
		if (ImGui::Button("Browse Audio")) browsing = true;

		if (track.channels != 0)
		{
			ImGui::SameLine();
			if (ImGui::Button("Edit"))
			{
				open = !open;
				offset = 0;
				knobReminder1 = false;
				knobReminder2 = false;
			}
			ImGui::Spacing();
			ImGui::Text("Options");
			if (ImGui::Checkbox("Mute", &mute)) SetVolume(volume);
			ImGui::Checkbox("Play on start", &playOnStart);
			if (ImGui::Checkbox("Loop", &loop)) SetLoop(loop);
			if (ImGui::Checkbox("Bypass FX", &bypass))
			{
				for (unsigned int i = 0; i < effects.size(); i++) effects[i]->ToggleBypass(!bypass);
			}
		}

		UpdatePlayState();

		if (open) DrawWindow(onWindow);

		if (browsing) browsing = BrowseAudio();
	}

	void End(Shape3D* afected)
	{
		audio->StopAudio(track.source);
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

			if (ImGui::BeginTable("Middle Table", 2))
			{
				ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, 180.0f); // Default to 100.0f
				ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				{
					// SLIDERS & KNOBS
					ImGui::Dummy(ImVec2{ 0.0f, 35.0f });
					ImGui::Dummy(ImVec2{ 3.8f, 0.0f }); ImGui::SameLine();
					if (mute) ImGui::BeginDisabled();
					ImGui::VSliderFloat("-0", ImVec2{ 15, 155 }, &volume, 0.0f, 100.0f, "");
					if (ImGui::IsItemDeactivatedAfterEdit()) SetVolume(volume);
					if (mute) ImGui::EndDisabled();
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
			}
			ImGui::EndTable();

			ImGui::Dummy(ImVec2{ 6.f, 0.0f }); ImGui::SameLine();

			if (ImGui::BeginTable("Bottom Table", 2))
			{
				ImGui::TableSetupColumn("one", ImGuiTableColumnFlags_WidthFixed, 170.0f); // Default to 100.0f
				ImGui::TableSetupColumn("two", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableNextRow();
				int selectId = -1;

				// EFFECT SELECTOR
				ImGui::TableSetColumnIndex(0);
				{
					ImGui::Text("Effects:");
					if (currEffect != 0)
					{
						ImGui::SameLine();
						if (ImGui::Button("Add"))
						{
							effects.push_back(CreateEffect(currEffect));
							fxTracker.erase(fxTracker.begin() + currEffect);
							currEffect--;
							totalEffects--;
						}
					}

					if (!effects.empty())
					{
						ImGui::SameLine();
						if (ImGui::Button("Delete")) RemoveEffect();
					}

					ImGui::Combo("##Effects", &currEffect, &fxTracker[0], fxTracker.size());

					bool select = false;
					bool clicked = false;
					for (unsigned int i = 0; i < effects.size(); i++)
					{
						Effect* e = effects[i];

						if (i % 2 != 0) ImGui::SameLine();
						bool click = ImGui::Selectable(e->GetName(), &e->selected, ImGuiSelectableFlags_None, ImVec2{ 85.0f, 12.0f});

						if (click && e->selected)
						{
							selectId = i;
							clicked = true;
						}
					}

					for (unsigned int i = 0; selectId != -1 && clicked && i < effects.size(); i++)
					{
						if (i == selectId) continue;
						effects[i]->selected = false;
					}
				}

				// PARAMETER TWICK
				ImGui::TableSetColumnIndex(1);
				{
					for (unsigned int i = 0; i < effects.size(); i++)
					{
						Effect* e = effects[i];
						if (e->selected)
						{
							e->Draw();
							break;
						}
					}
				}
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
				track.source = audio->CreateAudioSource(track.buffer, false);
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

	void SetLoop(bool active)
	{
		alSourcei(track.source, AL_LOOPING, active);
	}

	Effect* CreateEffect(int effect)
	{
		Effect* e = nullptr;

		const char* eName = fxTracker[effect];

		if ("EQ" == eName) e = new EQ(track.source, bypass);
		else if ("Compressor" == eName) e = new Compressor(track.source, bypass);
		else if ("Reverb" == eName) e = new Reverb(track.source, bypass);
		else if ("Distortion" == eName) e = new Distortion(track.source, bypass);
		else if ("Flanger" == eName) e = new Flanger(track.source, bypass);
		else if ("Delay" == eName) e = new Delay(track.source, bypass);
		else if ("Chorus"== eName) e = new Chorus(track.source, bypass);
		else if ("Auto Wah" == eName) e = new AutoWah(track.source, bypass);
		else if ("Ring Mod" == eName) e = new RingMod(track.source, bypass);
		else if ("Pitch Shift" == eName) e = new PitchShift(track.source, bypass);
		else if ("Freq Shift" == eName) e = new FreqShift(track.source, bypass);
		else if ("Vocal Morph" == eName) e = new VocalMorph(track.source, bypass);
		
		return e;
	}

	void UpdatePlayState()
	{
		ALint sourceState;
		alGetSourcei(track.source, AL_SOURCE_STATE, &sourceState);
		(sourceState == AL_PLAYING) ? play = true : play = false;
	}

	int GetEffectNameId(const char* eName)
	{
		if (audio->SameString("EQ", eName)) return 1;
		else if (audio->SameString("Compressor", eName)) return 2;
		else if (audio->SameString("Reverb", eName)) return 3;
		else if (audio->SameString("Distortion", eName)) return 4;
		else if (audio->SameString("Flanger", eName)) return 5;
		else if (audio->SameString("Delay", eName)) return 6;
		else if (audio->SameString("Chorus", eName)) return 7;
		else if (audio->SameString("Auto Wah", eName)) return 8;
		else if (audio->SameString("Ring Mod", eName)) return 9;
		else if (audio->SameString("Pitch Shift", eName)) return 10;
		else if (audio->SameString("Freq Shift", eName)) return 11;
		else if (audio->SameString("Vocal Morph", eName)) return 12;

		return -1;
	}

	const char* ReturnWrittedName(const char* eName)
	{
		if (audio->SameString("EQ", eName)) return "EQ";
		else if (audio->SameString("Compressor", eName)) return "Compressor";
		else if (audio->SameString("Reverb", eName)) return "Reverb";
		else if (audio->SameString("Distortion", eName)) return "Distortion";
		else if (audio->SameString("Flanger", eName)) return "Flanger";
		else if (audio->SameString("Delay", eName)) return "Delay";
		else if (audio->SameString("Chorus", eName)) return "Chorus";
		else if (audio->SameString("Auto Wah", eName)) return "Auto Wah";
		else if (audio->SameString("Ring Mod", eName)) return "Ring Mod";
		else if (audio->SameString("Pitch Shift", eName)) return "Pitch Shift";
		else if (audio->SameString("Freq Shift", eName)) return "Freq Shift";
		else if (audio->SameString("Vocal Morph", eName)) return "Vocal Morph";

		return "";
	}

	void RemoveEffect()
	{
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			if (effects[i]->selected)
			{
				Effect* e = effects[i];
				if (fxTracker.size() == 1) fxTracker.push_back(ReturnWrittedName(e->GetName()));
				else
				{
					int delId = GetEffectNameId(e->GetName());
					bool inserted = false;
					for (unsigned int a = 1; a < fxTracker.size(); a++)
					{
						if (delId < GetEffectNameId(fxTracker[a]))
						{
							fxTracker.insert(fxTracker.begin() + a, ReturnWrittedName(e->GetName()));
							inserted = true;
							break;
						}
					}
					if (!inserted) fxTracker.push_back(ReturnWrittedName(e->GetName()));
				}
				effects[i]->Disconnect(track.source);
				delete effects[i];
				effects.erase(effects.begin() + i);
				break;
			}
		}
	}

private: // Variables

	float volume = 100.0f, pan = 0, transpose = 0, offset = 0;

	bool knobReminder1 = false, knobReminder2 = false;
	bool play = false, browsing = false;
	bool playOnStart = true, loop = false, mute = false, bypass = false;

	int currEffect = 0;

	const char* fxNames[13] = { "-----", "EQ", "Compressor", "Reverb", "Distortion", "Flanger", "Delay", "Chorus", "Auto Wah", "Ring Mod", "Pitch Shift", "Freq Shift", "Vocal Morph" };
	std::vector<const char*> fxTracker = { "-----", "EQ", "Compressor", "Reverb", "Distortion", "Flanger", "Delay", "Chorus", "Auto Wah", "Ring Mod", "Pitch Shift", "Freq Shift", "Vocal Morph" };
	std::vector<Effect*> effects;
	unsigned int totalEffects = 13;

	Track track;
	AudioSystem* audio = nullptr;
};

#endif // !__AUDIO_SOURCE_COMPONENT_H__
