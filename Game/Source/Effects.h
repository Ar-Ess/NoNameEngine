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
		alEffecti(effect, AL_EFFECT_TYPE, id); // Error on the id
		if (alGetError() == AL_INVALID_VALUE)
		{
			printf("Reverb Effect not supported\n");
		}
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
	ALuint effect = 0, slot = 0;
	ALint id = 0;

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

		ImGui::PushItemWidth(70.0f);

		ImGui::SliderFloat("Wet       ", &gain, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_GAIN, gain);
		ImGui::SameLine();
		ImGui::SliderFloat("Diffusion ", &diffusion, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DIFFUSION, diffusion);
		ImGui::SameLine();
		ImGui::SliderFloat("Echo Time ", &echoTime, 0.075f, 0.25f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_TIME, echoTime);
		ImGui::SameLine();
		ImGui::SliderFloat("Rflc Gain ", &reflectionGain, 0.0f, 3.16f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_REFLECTIONS_GAIN, reflectionGain);
		ImGui::Spacing();

		ImGui::SliderFloat("Decay     ", &decayTime, 0.1f, 20.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DECAY_TIME, decayTime);
		ImGui::SameLine();
		ImGui::SliderFloat("Density   ", &density, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DENSITY, density);
		ImGui::SameLine();
		ImGui::SliderFloat("Echo Depth", &echoDepth, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_DEPTH, echoDepth);
		ImGui::SameLine();
		ImGui::SliderFloat("Rflc Delay", &reflectionDelay, 0.0f, 0.3f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_REFLECTIONS_DELAY, reflectionDelay);
		ImGui::Spacing();

		ImGui::SliderFloat("Delay Time", &lateReverbDelay, 0.0f, 0.1f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_DELAY, lateReverbDelay);
		ImGui::SameLine();
		ImGui::SliderFloat("Delay Gain", &lateReverbGain, 0.0f, 3.16f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_GAIN, lateReverbGain);
		ImGui::SameLine();
		ImGui::SliderFloat("Mod Time  ", &modulationTime, 0.04f, 4.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_MODULATION_TIME, modulationTime);
		ImGui::SameLine();
		ImGui::SliderFloat("Mod Depth ", &modulationDepth, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_MODULATION_DEPTH, modulationDepth);

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

	float density = 1.0f, diffusion = 1.0f, gain = 0.32f, /*gainhf, gainlf,*/ decayTime = 1.49f,
		/*decayhfRatio, decaylfRatio,*/ reflectionGain = 0.05f, reflectionDelay = 0.007f,
		lateReverbGain = 0.05f, lateReverbDelay = 0.011f, echoTime = 0.25f, echoDepth = 0.0f,
		modulationTime = 0.25f, modulationDepth = 0.0f/*, airAbsortion, roomRolloff,
		decayhfLimit, hfreference, lfreference*/;

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
	Distortion(ALint source, bool bypass) : Effect("Distortion", AL_EFFECT_DISTORTION)
	{
		Generate(source, bypass);
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
		ImGui::Text("Distortion"); ImGui::SameLine();
		if (ImGui::Checkbox("##Bypass", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(150.0f);

		ImGui::SliderFloat("Wet      ", &gain, 0.01f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_GAIN, gain);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Edge     ", &edge, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EDGE, edge);
		ImGui::Spacing();

		ImGui::SliderFloat("LP Cutoff", &lowpassCutoff, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_LOWPASS_CUTOFF, lowpassCutoff);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("EQ Center", &eqCenter, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EQCENTER, eqCenter);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("EQ Band W", &eqBandWidth, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EQBANDWIDTH, eqBandWidth);

		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	float edge = 0.2f, eqBandWidth = 3600.0f, gain = 0.05f,
		eqCenter = 3600.0f, lowpassCutoff = 8000.0f;

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

class Doppler : public Effect
{

};

#endif // !__EFFECTS_H__

