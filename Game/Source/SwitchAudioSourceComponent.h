#ifndef __SWITCH_AUDIO_SOURCE_COMPONENT_H__
#define __SWITCH_AUDIO_SOURCE_COMPONENT_H__

#include "Component.h"
#include "Effects.h"
#include "External/ImGuiFileDialog/ImGuiFileDialog.h"

struct TrackInstance
{
	bool Start(AudioSystem* audio)
	{
		knobReminder1 = false;
		knobReminder2 = false;
		if (playOnStart) audio->PlayAudio(track.source);
		return playOnStart;
	}

	void Delete(AudioSystem* audio)
	{
		fxTracker.clear();
		for (unsigned int i = 0; i < effects.size(); i++)
		{
			audio->StopAudio(track.source);
			effects[i]->Disconnect(track.source);
			delete effects[i];
		}
		effects.clear();
	}

	bool IsLoaded()
	{
		return (track.channels != 0);
	}

	Track track;
	std::vector<Effect*> effects;
	bool play = false;
	bool playOnStart = false, loop = false, mute = false, bypass = false;
	bool knobReminder1 = false, knobReminder2 = false;
	float volume = 100.0f, pan = 0, transpose = 0, offset = 0;
	int currEffect = 0;

	const char* fxNames[13] = { "-----", "EQ", "Compressor", "Reverb", "Distortion", "Flanger", "Delay", "Chorus", "Auto Wah", "Ring Mod", "Pitch Shift", "Freq Shift", "Vocal Morph" };
	std::vector<const char*> fxTracker = { "-----", "EQ", "Compressor", "Reverb", "Distortion", "Flanger", "Delay", "Chorus", "Auto Wah", "Ring Mod", "Pitch Shift", "Freq Shift", "Vocal Morph" };
	unsigned int totalEffects = 13;
};

class SwitchAudioSourceComponent : public Component
{
public:
	SwitchAudioSourceComponent(Timer* timer, AudioSystem* audioSystem, ModuleInput* input) : Component("Switch Audio Source", ComponentID::AUDIO_SOURCE_COMPONENT)
	{
		gameTimer = timer;
		audio = audioSystem;
		this->input = input;

		tracks.push_back({});
		tracks.push_back({});

		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			SetVolume(tracks[i].volume, &tracks[i]);
			SetPanning(tracks[i].pan, &tracks[i]);
			SetTranspose(tracks[i].transpose, &tracks[i]);
			SetLoop(tracks[i].loop, &tracks[i]);
		}
	}
	~SwitchAudioSourceComponent()
	{
		StopAllTracks();
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			tracks[i].Delete(audio);
		}
		this->title.clear();
	}

	void Start(Shape3D* afected)
	{
		StopAllTracks();
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			if (tracks[i].Start(audio)) playingTrack = &tracks[i];
		}
		switching = false;
		browsing = false;
		switchTime = 0;
	}

	void Update(Shape3D* afected)
	{
		SwitchFade(fadeTime);

		if (gameTimer->GetTimerState() == STOPPED) return;

		//if (gameTimer->GetTimerState() == PAUSED)
		//{
		//	audio->PauseAudio(playingTrack->track.source);
		//	return;
		//}

		//audio->ResumeAudio(playingTrack->track.source);
		
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			TrackInstance* index = &tracks[i];
			if (gameTimer->GetTimerState() == PAUSED)
			{
				audio->PauseAudio(index->track.source);
				if (switching) pauseDifference = gameTimer->RealReadSec() - switchTime;
				continue;
			}

			audio->ResumeAudio(index->track.source);
		}

		if (input->GetKey(SDL_SCANCODE_F) == KEY_REPEAT)
		{
			if (input->GetKey(SDL_SCANCODE_1) == KEY_DOWN) SwitchTrack(0);
			if (input->GetKey(SDL_SCANCODE_2) == KEY_DOWN) SwitchTrack(1);
			if (input->GetKey(SDL_SCANCODE_3) == KEY_DOWN) SwitchTrack(2);
		}

		UpdatePlayState();
	}

	void Draw(Shape3D* affected, bool* onWindow = nullptr)
	{
		if (ImGui::BeginTable("SwitchAudioSourceComp", 2))
		{
			ImGui::TableSetupColumn("SAST10", ImGuiTableColumnFlags_WidthFixed, 2);
			ImGui::TableSetupColumn("SAST11");

			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::TableSetColumnIndex(1);

			// SWITCH BUTTON
			if (ImGui::Button("Switch To") && !switching)
			{
				SwitchTrack(nextSwitchTrack - 1);
			}
			ImGui::SameLine();
			ImGui::DragInt("##Switch", &nextSwitchTrack, 0.1f, 1, tracks.size(), "Track %d");

			ImGui::Spacing();

			ImGui::Text("Fade Time "); ImGui::SameLine(73);
			AddHelper("?", "Fade Time:\nModifying this variable changes the time the\ntracks will be in switching mode. The higher the\nvalue, the slower it will fade and viceversa.\n\nOperations:\nShift-Click for precision changing", false);
			ImGui::SameLine(87);
			
			float speed = 1.0f;
			((input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT) || (input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_REPEAT)) ? speed = 0.001f : speed = 0.2f;
			ImGui::DragFloat("##FadeTime", &fadeTime, speed, 0.005f, 10.0f, "%.2f sec");

			ImGui::Spacing();
			ImGui::Spacing();

			// STOP TRACK
			if (IsAnyAudioPlaying())
			{
				if (ImGui::Button("Stop Track"))
				{
					StopAllTracks();
					playingTrack = nullptr;
				}

				ImGui::Spacing();
				ImGui::Spacing();
			}

			// TRACK LIST

			ImGui::PushItemWidth(80);
			ImGui::Text("Tracks"); ImGui::SameLine(); ImGui::SliderInt("##TrackCount", &totalTracks, 2, 8);
			if (ImGui::IsItemDeactivatedAfterEdit() && totalTracks != tracks.size())
			{
				while (totalTracks < tracks.size())
				{
					tracks.erase(tracks.end() - 1);
				}

				while (totalTracks > tracks.size())
				{
					tracks.push_back({});
					TrackInstance* last = &tracks.at(tracks.size() - 1);
					SetVolume(last->volume, last);
					SetPanning(last->pan, last);
					SetTranspose(last->transpose, last);
					SetLoop(last->loop, last);
					if (totalTracks == 10 && tracks.size() == 10) last->bypass = true;
				}
			}
			ImGui::PopItemWidth();
			ImGui::Spacing();
			for (int i = 0; i < tracks.size(); i++)
			{
				ImGui::PushID(i + 50);
				TrackInstance* index = &tracks[i];
				if (tracks.size() > 2)
				{
					if (ImGui::Button("X"))
					{
						totalTracks--;
						if (nextSwitchTrack > 1) nextSwitchTrack--;
						audio->StopAudio(index->track.source);
						tracks.at(i).Delete(audio);
						tracks.erase(tracks.begin() + i);
						ImGui::PopID();
						ImGui::SameLine();
						continue;
					}
					ImGui::SameLine();
				}
				std::string trackName = trackNaming[i];
				if (!index->IsLoaded()) // false
				{
					ImGui::PushStyleColor(ImGuiCol_Header, ImVec4{ 0.7f, 0, 0, 0.2f });
					trackName += " (no audio)";
				}

				if (ImGui::CollapsingHeader(trackName.c_str(), ImGuiTreeNodeFlags_Framed))
				{
					if (ImGui::BeginTable("TrackInsights", 2))
					{
						ImGui::TableSetupColumn("SAST20", ImGuiTableColumnFlags_WidthFixed, 3);
						ImGui::TableSetupColumn("SAST21");

						ImGui::TableNextRow();

						ImGui::TableSetColumnIndex(0);
						ImGui::TableSetColumnIndex(1);

						if (ImGui::Button("Browse Audio") && !browsing)
						{
							browsing = true;
							currentBrowsingTrack = i;
						}

						if (index->IsLoaded()) // True
						{
							ImGui::SameLine();
							if (ImGui::Button("Edit"))
							{
								currentTrackEditor = i;
								open = true;
								index->offset = 0;
								index->knobReminder1 = false;
								index->knobReminder2 = false;

								StopAllTracks();
							}
							ImGui::Spacing();
							ImGui::Text("Options");
							if (ImGui::Checkbox("Mute", &index->mute)) SetVolume(index->volume, &tracks[i]);
							if (ImGui::Checkbox("Play on start", &index->playOnStart) && index->playOnStart)
							{
								DisablePlayOnStart();
								index->playOnStart = true;
							}
							AddHelper("?", "Play On Start:\nEnabling this option will play the audio\nat the start of the game.\nOnly one track can be checked with this option.\nWhenever you check it, the option will be\ndisabled in other tracks automatically.", true);
							if (ImGui::Checkbox("Loop", &index->loop)) SetLoop(index->loop, &tracks[i]);
							if (ImGui::Checkbox("Bypass FX", &index->bypass))
							{
								for (unsigned int i = 0; i < index->effects.size(); i++) index->effects[i]->ToggleBypass(!index->bypass);
							}
						}
					}
					ImGui::EndTable();
				}
				if (!index->IsLoaded()) ImGui::PopStyleColor(); // False
				ImGui::PopID();
			}

		}
		ImGui::EndTable();

		UpdatePlayState();

		if (open) DrawWindow(onWindow, &tracks[currentTrackEditor]);

		if (browsing) browsing = BrowseAudio(&tracks[currentBrowsingTrack]);
	}

	void End(Shape3D* afected)
	{
		for (unsigned int i = 0; i < tracks.size(); i++) audio->StopAudio(tracks[i].track.source);
	}

private: // Useful methods

	void Play(int trackIndex)
	{
		if (trackIndex >= tracks.size()) return;

		TrackInstance* index = &tracks[trackIndex];

		audio->PlayAudio(index->track.source);
	}

	void Stop(int trackIndex)
	{
		if (trackIndex >= tracks.size()) return;

		TrackInstance* index = &tracks[trackIndex];

		audio->StopAudio(index->track.source);
	}

	void Pause(int trackIndex)
	{
		if (trackIndex >= tracks.size()) return;

		TrackInstance* index = &tracks[trackIndex];

		audio->PauseAudio(index->track.source);
	}

	void Resume(int trackIndex)
	{
		if (trackIndex >= tracks.size()) return;

		TrackInstance* index = &tracks[trackIndex];

		audio->ResumeAudio(index->track.source);
	}

	void SetVolume(float volume, int trackIndex)
	{
		if (trackIndex >= tracks.size()) return;

		TrackInstance* index = &tracks[trackIndex];

		if (index->mute)
		{
			alSourcef(index->track.source, AL_GAIN, 0);
			return;
		}

		volume = Pow(volume, 2.5f) / 1000.0f;
		if (volume > 99.0f) volume = 100.0f;
		alSourcef(index->track.source, AL_GAIN, volume / 100);
	}

	void SwitchTrack(int newTrackIndex)
	{
		oldTrack = GetPlayingTrack();
		if (newTrackIndex >= tracks.size()) return;
		newTrack = &tracks[newTrackIndex];

		if (oldTrack != nullptr && !newTrack->play && newTrack->IsLoaded())
		{
			switching = true;
			switchTime = gameTimer->RealReadSec();
		}
	}

	bool IsAnyAudioPlaying()
	{
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			if (!tracks[i].IsLoaded()) continue;

			return tracks[i].play;
		}
	}

	TrackInstance* GetPlayingTrack()
	{
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			if (tracks[i].play) return &tracks[i];
		}

		return nullptr;
	}

private: // Meta Methods (Do not use)

	void DrawWindow(bool* onWindow, TrackInstance* index)
	{
		if (ImGui::Begin(title.c_str(), &open))
		{
			if (onWindow != nullptr && !*onWindow) *onWindow = ImGui::IsWindowHovered();
			bool mono = index->track.channels == 1;

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
					ImGui::Text("%s", index->track.name.c_str());
					ImGui::Spacing();
					ImGui::Text("%d kHz | %d-Bits", index->track.sampleRate, index->track.bits);
					ImGui::Spacing();
					ImGui::Text("%d Channels", index->track.channels);
					if (!mono)
					{
						ImGui::SameLine(); ImGui::Text(" (No Pan)");
					}
				}

				ImGui::TableSetColumnIndex(1);
				{
					char t1[] = { "Play" };
					char t2[] = { "Stop" };
					char* t = index->play ? t = t2 : t = t1;
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::Dummy(ImVec2{ width, 0.0f }); ImGui::SameLine();
					if (ImGui::Button(t))
					{
						float time = index->track.duration * index->offset;
						bool p = false;
						index->play ? audio->StopAudio(index->track.source) : p = true;
						if (p)
						{
							StopAllTracks();
							audio->PlayAudio(index->track.source, time);
							playingTrack = index;
						}
						else
						{
							playingTrack = nullptr;
						}

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
					if (index->mute) ImGui::BeginDisabled();
					ImGui::VSliderFloat("-0", ImVec2{ 15, 155 }, &index->volume, 0.0f, 100.0f, "");
					if (ImGui::IsItemDeactivatedAfterEdit()) SetVolume(index->volume, index);
					if (index->mute) ImGui::EndDisabled();
					ImGui::SameLine(0.0f, 20.0f);
					if (ImGui::BeginTable("Column Table", 2))
					{
						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
						ImGui::Dummy(ImVec2{ 0.0f, 6.0f });

						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
						ImGui::Dummy(ImVec2{ 4.5f, 0.0f }); ImGui::SameLine();
						if (ImGui::Knob("L / R", &index->pan, -0.5, 0.5, false, mono, 2.5f, &index->knobReminder2) && mono) SetPanning(index->pan, index);
						ImGui::Dummy(ImVec2{ 0.0f, 10.0f });

						ImGui::TableNextRow(); ImGui::TableSetColumnIndex(0);
						if (ImGui::Knob("Transpose", &index->transpose, -24.0, 24.0, false, true, -10, &index->knobReminder1)) SetTranspose(index->transpose, index);
					}
					ImGui::EndTable();
				}

				ImGui::TableSetColumnIndex(1);
				{
					// VOLUME GRAPHIC
					float a[10] = { 10.0f, 9.0f, 8.0f, 7.0f, 6.0f, 5.0f, 4.0f, 3.0f, 2.0f, 1.0f };
					float width = ImGui::GetWindowWidth();
					ImGui::PushItemWidth(width - 220);
					ImGui::SliderFloat("##offset", &index->offset, 0.0f, 1.0f, "");
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
					if (index->currEffect != 0)
					{
						ImGui::SameLine();
						if (ImGui::Button("Add"))
						{
							index->effects.push_back(CreateEffect(index->currEffect, index));
							index->fxTracker.erase(index->fxTracker.begin() + index->currEffect);
							index->currEffect--;
							index->totalEffects--;
						}
					}

					if (!index->effects.empty())
					{
						ImGui::SameLine();
						if (ImGui::Button("Delete")) RemoveEffect(index);
					}

					ImGui::Combo("##Effects", &index->currEffect, &index->fxTracker[0], index->fxTracker.size());

					bool select = false;
					bool clicked = false;
					for (unsigned int i = 0; i < index->effects.size(); i++)
					{
						Effect* e = index->effects[i];

						if (i % 2 != 0) ImGui::SameLine();
						bool click = ImGui::Selectable(e->GetName(), &e->selected, ImGuiSelectableFlags_None, ImVec2{ 85.0f, 12.0f });

						if (click && e->selected)
						{
							selectId = i;
							clicked = true;
						}
					}

					for (unsigned int i = 0; selectId != -1 && clicked && i < index->effects.size(); i++)
					{
						if (i == selectId) continue;
						index->effects[i]->selected = false;
					}
				}

				// PARAMETER TWICK
				ImGui::TableSetColumnIndex(1);
				{
					for (unsigned int i = 0; i < index->effects.size(); i++)
					{
						Effect* e = index->effects[i];
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

		if (!open && prevOpen) audio->StopAudio(index->track.source);

		prevOpen = open;
	}

	bool BrowseAudio(TrackInstance* index)
	{
		bool ret = true;

		// open Dialog Simple
		ImGuiFileDialog::Instance()->OpenDialog("ChooseFileDlgKey", "Choose an Audio File", ".mp3,.wav,.flac", ".");

		//display
		if (ImGuiFileDialog::Instance()->Display("ChooseFileDlgKey", ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove))
		{
			// action if OK
			if (ImGuiFileDialog::Instance()->IsOk() == true)
			{
				std::string path = ImGuiFileDialog::Instance()->GetCurrentPath() + "\\" + ImGuiFileDialog::Instance()->GetCurrentFileName();
				index->track = audio->LoadAudio(path.c_str());
				index->track.source = audio->CreateAudioSource(index->track.buffer, false);
				SetVolume(index->volume, index);
				SetPanning(index->pan, index);
				SetTranspose(index->transpose, index);
				ret = false;
				ImGuiFileDialog::Instance()->Close();
			}
			else
			{
				ret = false;
				ImGuiFileDialog::Instance()->Close();
			}
		}

		return ret;
	}

	void SetVolume(float volume, TrackInstance* index)
	{
		if (index->mute)
		{
			alSourcef(index->track.source, AL_GAIN, 0);
			return;
		}

		volume = Pow(volume, 2.5f) / 1000.0f;
		if (volume > 99.0f) volume = 100.0f;
		alSourcef(index->track.source, AL_GAIN, volume / 100);
	}

	void SetPanning(float pan, TrackInstance* index)
	{
		pan = pan * -1;
		alSource3f(index->track.source, AL_POSITION, pan, 0, -sqrtf(1.0f - pan * pan));
	}

	void SetTranspose(float transpose, TrackInstance* index)
	{
		transpose = exp(0.0577623f * transpose);
		if (transpose > 4.0f) transpose = 4.0f;
		if (transpose < 0.25f) transpose = 0.25f;
		if (transpose > 0.98f && transpose < 1.02f) transpose = 1.0f;
		alSourcef(index->track.source, AL_PITCH, transpose);
	}

	void SetLoop(bool active, TrackInstance* index)
	{
		alSourcei(index->track.source, AL_LOOPING, active);
	}

	Effect* CreateEffect(int effect, TrackInstance* index)
	{
		Effect* e = nullptr;

		const char* eName = index->fxTracker[effect];

		if ("EQ" == eName) e = new EQ(index->track.source, index->bypass);
		else if ("Compressor" == eName) e = new Compressor(index->track.source, index->bypass);
		else if ("Reverb" == eName) e = new Reverb(index->track.source, index->bypass);
		else if ("Distortion" == eName) e = new Distortion(index->track.source, index->bypass);
		else if ("Flanger" == eName) e = new Flanger(index->track.source, index->bypass);
		else if ("Delay" == eName) e = new Delay(index->track.source, index->bypass);
		else if ("Chorus" == eName) e = new Chorus(index->track.source, index->bypass);
		else if ("Auto Wah" == eName) e = new AutoWah(index->track.source, index->bypass);
		else if ("Ring Mod" == eName) e = new RingMod(index->track.source, index->bypass);
		else if ("Pitch Shift" == eName) e = new PitchShift(index->track.source, index->bypass);
		else if ("Freq Shift" == eName) e = new FreqShift(index->track.source, index->bypass);
		else if ("Vocal Morph" == eName) e = new VocalMorph(index->track.source, index->bypass);

		return e;
	}

	void UpdatePlayState()
	{
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			TrackInstance* index = &tracks[i];
			if (index->IsLoaded()) // True
			{
				ALint sourceState;
				alGetSourcei(index->track.source, AL_SOURCE_STATE, &sourceState);
				(sourceState == AL_PLAYING) ? index->play = true : index->play = false;
			}
			if (index->play)
			{
				int a = 0;
			}
		}
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

	void RemoveEffect(TrackInstance* index)
	{
		for (unsigned int i = 0; i < index->effects.size(); i++)
		{
			if (index->effects[i]->selected)
			{
				Effect* e = index->effects[i];
				if (index->fxTracker.size() == 1) index->fxTracker.push_back(ReturnWrittedName(e->GetName()));
				else
				{
					int delId = GetEffectNameId(e->GetName());
					bool inserted = false;
					for (unsigned int a = 1; a < index->fxTracker.size(); a++)
					{
						if (delId < GetEffectNameId(index->fxTracker[a]))
						{
							index->fxTracker.insert(index->fxTracker.begin() + a, ReturnWrittedName(e->GetName()));
							inserted = true;
							break;
						}
					}
					if (!inserted) index->fxTracker.push_back(ReturnWrittedName(e->GetName()));
				}
				index->effects[i]->Disconnect(index->track.source);
				delete index->effects[i];
				index->effects.erase(index->effects.begin() + i);
				break;
			}
		}
	}

	// Function wich actually updates the fade
	void SwitchFade(float fadeSeconds)
	{
		// Look if both tracks exist
		if (switching)
		{
			if (!newTrack->play)
			{
				audio->PlayAudio(newTrack->track.source);
				alSourcef(newTrack->track.source, AL_GAIN, 0.0f);
			}

			if (gameTimer->GetTimerState() == PAUSED)
			{
				switchTime = gameTimer->RealReadSec() - pauseDifference;
				return;
			}

			float t = gameTimer->RealReadSec() - switchTime;
			float d = fadeSeconds;
			if (t > d)
			{
				switching = false; // Stop source
				audio->StopAudio(oldTrack->track.source);
				SetVolume(oldTrack->volume, oldTrack);
				SetVolume(newTrack->volume, newTrack);
				playingTrack = newTrack;
				oldTrack = nullptr;
				newTrack = nullptr;
				pauseDifference = 0;
			}
			else
			{
				float volume = oldTrack->volume;
				volume = Pow(volume, 2.5f) / 1000.0f;
				if (volume > 99.0f) volume = 100.0f;
				volume = (volume / 100) * (1.0f - t / d);
				alSourcef(oldTrack->track.source, AL_GAIN, volume);

				float nvolume = newTrack->volume;
				nvolume = Pow(nvolume, 2.5f) / 1000.0f;
				if (nvolume > 99.0f) nvolume = 100.0f;
				nvolume = (nvolume / 100) * (t / d);
				alSourcef(newTrack->track.source, AL_GAIN, nvolume);
			}
		}
	}

	void DisablePlayOnStart()
	{
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			if (!tracks[i].IsLoaded()) continue;

			tracks[i].playOnStart = false;
		}
	}

	void StopAllTracks()
	{
		for (unsigned int i = 0; i < tracks.size(); i++)
		{
			if (!tracks[i].IsLoaded()) continue;

			audio->StopAudio(tracks[i].track.source);
		}
	}

private: // Variables

	bool browsing = false;
	bool switching = false;

	int totalTracks = 2;
	int currentTrackEditor = 0;
	int currentBrowsingTrack = 0;

	int nextSwitchTrack = 1;

	float fadeTime = 2.0f;

	TrackInstance* oldTrack = nullptr;
	TrackInstance* newTrack = nullptr;
	TrackInstance* playingTrack = nullptr;
	float switchTime = 0;
	float pauseDifference = 0;

	std::vector<TrackInstance> tracks;

	std::string trackNaming[10] = {"Track 1", "Track 2", "Track 3", "Track 4", "Track 5", "Track 6", "Track 7", "Track 8", "Track 9", "Track 10"};
	AudioSystem* audio = nullptr;
	ModuleInput* input = nullptr;

};

#endif // !__SWITCH_AUDIO_SOURCE_COMPONENT_H__