#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "AudioSystem.h"

class Effect
{
public:
	virtual ~Effect() {}

	virtual void Start()
	{
	}

	virtual void Update()
	{
	}

	virtual void Draw()
	{
	}

	virtual void End()
	{

	}

public: // Methods

	const char* GetName() const
	{
		return name.c_str();
	}

	void ToggleBypass(bool toggle)
	{
		toggle ? alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, (ALint)effect) : alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
		nobypass = toggle;
	
	}

	void Disconnect(ALuint source)
	{
		alSource3i(source, AL_AUXILIARY_SEND_FILTER, AL_EFFECTSLOT_NULL, 0, AL_FILTER_NULL);
		alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, AL_EFFECT_NULL);
		alDeleteAuxiliaryEffectSlots(1, &slot);
		alDeleteEffects(1, &effect);
	}

public: // Variables

	bool selected = false;
	bool nobypass = true;

protected:

	Effect(const char* xname, ALint ID, bool bypass = false)
	{
		name.clear();
		name = xname;
		id = ID;
		nobypass = !bypass;
	}

	void Generate(ALuint source = 0, bool bypass = false)
	{
#define LOAD_PROC(T, x)  ((x) = (T)alGetProcAddress(#x))

		LOAD_PROC(LPALGENEFFECTS, alGenEffects);
		LOAD_PROC(LPALDELETEEFFECTS, alDeleteEffects);
		LOAD_PROC(LPALISEFFECT, alIsEffect);
		LOAD_PROC(LPALEFFECTI, alEffecti);
		LOAD_PROC(LPALEFFECTIV, alEffectiv);
		LOAD_PROC(LPALEFFECTF, alEffectf);
		LOAD_PROC(LPALEFFECTFV, alEffectfv);
		LOAD_PROC(LPALGETEFFECTI, alGetEffecti);
		LOAD_PROC(LPALGETEFFECTIV, alGetEffectiv);
		LOAD_PROC(LPALGETEFFECTF, alGetEffectf);
		LOAD_PROC(LPALGETEFFECTFV, alGetEffectfv);

		LOAD_PROC(LPALGENAUXILIARYEFFECTSLOTS, alGenAuxiliaryEffectSlots);
		LOAD_PROC(LPALDELETEAUXILIARYEFFECTSLOTS, alDeleteAuxiliaryEffectSlots);
		LOAD_PROC(LPALISAUXILIARYEFFECTSLOT, alIsAuxiliaryEffectSlot);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTI, alAuxiliaryEffectSloti);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTIV, alAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTF, alAuxiliaryEffectSlotf);
		LOAD_PROC(LPALAUXILIARYEFFECTSLOTFV, alAuxiliaryEffectSlotfv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTI, alGetAuxiliaryEffectSloti);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTIV, alGetAuxiliaryEffectSlotiv);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTF, alGetAuxiliaryEffectSlotf);
		LOAD_PROC(LPALGETAUXILIARYEFFECTSLOTFV, alGetAuxiliaryEffectSlotfv);
#undef LOAD_PROC

		alGenEffects(1, &effect);
		alEffecti(effect, AL_EFFECT_TYPE, id);
		alGenAuxiliaryEffectSlots(1, &slot);
		ToggleBypass(!bypass);
		alSource3i(source, AL_AUXILIARY_SEND_FILTER, (ALint)slot, 0, AL_FILTER_NULL);
	}

	void SetEffectValue(ALuint param, float value)
	{
		alEffectf(effect, param, value);
		alAuxiliaryEffectSloti(slot, AL_EFFECTSLOT_EFFECT, (ALint)effect);
	}

	std::string name = "Effect";
	ALuint id = 0, effect = 0, slot = 0;

	/* Effect object functions */
	LPALGENEFFECTS alGenEffects;
	LPALDELETEEFFECTS alDeleteEffects;
	LPALISEFFECT alIsEffect;
	LPALEFFECTI alEffecti;
	LPALEFFECTIV alEffectiv;
	LPALEFFECTF alEffectf;
	LPALEFFECTFV alEffectfv;
	LPALGETEFFECTI alGetEffecti;
	LPALGETEFFECTIV alGetEffectiv;
	LPALGETEFFECTF alGetEffectf;
	LPALGETEFFECTFV alGetEffectfv;

	/* Auxiliary Effect Slot object functions */
	LPALGENAUXILIARYEFFECTSLOTS alGenAuxiliaryEffectSlots;
	LPALDELETEAUXILIARYEFFECTSLOTS alDeleteAuxiliaryEffectSlots;
	LPALISAUXILIARYEFFECTSLOT alIsAuxiliaryEffectSlot;
	LPALAUXILIARYEFFECTSLOTI alAuxiliaryEffectSloti;
	LPALAUXILIARYEFFECTSLOTIV alAuxiliaryEffectSlotiv;
	LPALAUXILIARYEFFECTSLOTF alAuxiliaryEffectSlotf;
	LPALAUXILIARYEFFECTSLOTFV alAuxiliaryEffectSlotfv;
	LPALGETAUXILIARYEFFECTSLOTI alGetAuxiliaryEffectSloti;
	LPALGETAUXILIARYEFFECTSLOTIV alGetAuxiliaryEffectSlotiv;
	LPALGETAUXILIARYEFFECTSLOTF alGetAuxiliaryEffectSlotf;
	LPALGETAUXILIARYEFFECTSLOTFV alGetAuxiliaryEffectSlotfv;
};

class Reverb : public Effect
{
public:
	Reverb(ALuint source, bool bypass) : Effect("Reverb", AL_EFFECT_EAXREVERB, bypass)
	{
		Generate(source, bypass);
	}
	~Reverb() 
	{
	}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Reverb"); ImGui::SameLine();
		if (ImGui::Checkbox("##Bypass", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(100.0f);

		ImGui::SliderFloat("Wet", &gain, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_GAIN, gain);
		ImGui::SameLine();
		ImGui::SliderFloat("Diffusion", &diffusion, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DIFFUSION, diffusion);
		ImGui::SameLine();
		ImGui::SliderFloat("Echo Time", &echoTime, 0.0f, 200.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_TIME, echoTime);
		ImGui::Spacing();

		ImGui::SliderFloat("Decay", &decayTime, 0.0f, 100.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DECAY_TIME, decayTime);
		ImGui::SameLine();
		ImGui::SliderFloat("Density", &density, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DENSITY, density);
		ImGui::SameLine();
		ImGui::SliderFloat("Echo Time", &echoTime, 0.0f, 200.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_TIME, echoTime);
		ImGui::Spacing();

		ImGui::SliderFloat("Delay Time", &lateReverbDelay, 0.0f, 200.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_DELAY, lateReverbDelay);
		ImGui::SameLine();
		ImGui::SliderFloat("Delay Gain", &lateReverbGain, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_GAIN, lateReverbGain);
		ImGui::SameLine();
		ImGui::SliderFloat("Echo Depth", &echoDepth, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_DEPTH, echoDepth);

		ImGui::PopItemWidth();

		/*
		alEffectf(effect, AL_EAXREVERB_GAINHF,                gainhf);
		alEffectf(effect, AL_EAXREVERB_GAINLF,                gainlf);
		alEffectf(effect, AL_EAXREVERB_DECAY_HFRATIO,         decayhfRatio);
		alEffectf(effect, AL_EAXREVERB_DECAY_LFRATIO,         decaylfRatio);
		alEffectf(effect, AL_EAXREVERB_REFLECTIONS_GAIN,      reflectionGain);
		alEffectf(effect, AL_EAXREVERB_REFLECTIONS_DELAY,     reflectionDelay);
		alEffectf(effect, AL_EAXREVERB_MODULATION_TIME,       modulationTime);
		alEffectf(effect, AL_EAXREVERB_MODULATION_DEPTH,      modulationDepth);
		alEffectf(effect, AL_EAXREVERB_AIR_ABSORPTION_GAINHF, airAbsortion);
		alEffectf(effect, AL_EAXREVERB_ROOM_ROLLOFF_FACTOR,   roomRolloff);
		alEffectf(effect, AL_EAXREVERB_DECAY_HFLIMIT,         decayhfLimit);

		alEffectf(effect, AL_EAXREVERB_HFREFERENCE,           hfreference);
		alEffectf(effect, AL_EAXREVERB_LFREFERENCE,           lfreference);*/
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	float density, diffusion, gain = 0.0f, gainhf, gainlf, decayTime,
		decayhfRatio, decaylfRatio, reflectionGain, reflectionDelay,
		lateReverbGain, lateReverbDelay, echoTime, echoDepth,
		modulationTime, modulationDepth, airAbsortion, roomRolloff,
		decayhfLimit, hfreference, lfreference;

};

class Delay : public Effect
{
public:
	Delay() : Effect("Delay", AL_EFFECT_ECHO)
	{
		Generate();
	}
	~Delay() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Delay");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class Distortion : public Effect
{
public:
	Distortion() : Effect("Distortion", AL_EFFECT_DISTORTION)
	{
		Generate();
	}
	~Distortion() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Distortion");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class Flanger : public Effect
{
public:
	Flanger() : Effect("Flanger", AL_EFFECT_FLANGER)
	{
		Generate();
	}
	~Flanger() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Flanger");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class Chorus : public Effect
{
public:
	Chorus() : Effect("Chorus", AL_EFFECT_CHORUS)
	{
		Generate();
	}
	~Chorus() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Chorus");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class AutoWah : public Effect
{
public:
	AutoWah() : Effect("Auto Wah", AL_EFFECT_AUTOWAH)
	{
		Generate();
	}
	~AutoWah() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Auto Wah");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class EQ : public Effect
{
public:
	EQ() : Effect("EQ", AL_EFFECT_EQUALIZER)
	{
		Generate();
	}
	~EQ() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("EQ");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class Compressor : public Effect
{
public:
	Compressor() : Effect("Compressor", AL_EFFECT_COMPRESSOR)
	{
		Generate();
	}
	~Compressor() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Compressor");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class FreqShift : public Effect
{
public:
	FreqShift() : Effect("Freq Shift", AL_EFFECT_FREQUENCY_SHIFTER)
	{
		Generate();
	}
	~FreqShift() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Frequency Shifter");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class PitchShift : public Effect
{
public:
	PitchShift() : Effect("Pitch Shift", AL_EFFECT_PITCH_SHIFTER)
	{
		Generate();
	}
	~PitchShift() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Pitch Shifter");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class RingMod : public Effect
{
public:
	RingMod() : Effect("Ring Mod", AL_EFFECT_RING_MODULATOR)
	{
		Generate();
	}
	~RingMod() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Ring Modulation");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class VocalMorph : public Effect
{
public:
	VocalMorph() : Effect("Vocal Morph", AL_EFFECT_VOCAL_MORPHER)
	{
		Generate();
	}
	~VocalMorph() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
		ImGui::Text("Vocal Morpher");
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

#endif // !__EFFECTS_H__

