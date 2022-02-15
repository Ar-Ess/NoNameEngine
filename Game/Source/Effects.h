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

	void AddHelper(const char* title = "(?)", const char* desc = "", bool sameLine = false)
	{
		if (sameLine) ImGui::SameLine();
		ImGui::TextDisabled(title);
		if (ImGui::IsItemHovered())
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
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

		if (ImGui::Checkbox("##BypassR", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(70.0f);

		//REVERB
		ImGui::SliderFloat("##ReverbWet", &gain, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_GAIN, gain);
		AddHelper("Wet", "WET PROPERTY:\nThe Reverb Gain property controls the volume of the reverb track. Setting it to maximum will set full reverb track.\nSetting it to minimum will not apply reverb", true);
		ImGui::SameLine(); ImGui::Text("      "); ImGui::SameLine();

		//DIFFUSION
		ImGui::SliderFloat("##ReverbDiffusion  ", &diffusion, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DIFFUSION, diffusion);
		AddHelper("Diffusion  ", "DIFFUSION PROPERTY:\nThe Reverb Diffusion property controls the echo density in the reverberation decay.\nReducing diffusion gives the reverberation a more “grainy” character that is especially noticeable with percussive sound sources.\nIf you set a diffusion value of 0.0, the later reverberation sounds like a succession of distinct echoes.", true);
		ImGui::SameLine();

		//ECHO TIME
		ImGui::SliderFloat("##ReverbEchoTime", &echoTime, 0.075f, 0.25f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_TIME, echoTime);
		AddHelper("Echo Time", "ECHO TIME PROPERTY:\nEcho Time controls the rate at which the cyclic echo repeats itself along the reverberation decay.\nFor example, the default setting for Echo Time is 250 ms, causing the echo to occur 4 times per second.\nTherefore, if you were to clap your hands in this type of environment, you will hear four repetitions of clap per second.", true);
		ImGui::SameLine(); ImGui::Text(" "); ImGui::SameLine();

		//REFLECT GAIN
		ImGui::SliderFloat("##ReverbRflcGain", &reflectionGain, 0.0f, 3.16f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_REFLECTIONS_GAIN, reflectionGain);
		AddHelper("Rflc Gain", "REFLECT GAIN PROPERTY:\nThe Reflections Gain property controls the overall amount of initial reflections relative to the Gain property.\nYou can increase the amount of initial reflections to simulate a more narrow space or closer walls,\nespecially effective if you associate the initial reflections increase with a reduction in reflections delays by lowering the value of the Reflection Delay property.\nTo simulate open or semi - open environments, you can maintain the amount of early reflections while reducing the value of the Late Reverb Gain property, which controls later reflections. ", true);
		ImGui::Spacing();

		//DECAY
		ImGui::SliderFloat("##ReverbDecay", &decayTime, 0.1f, 20.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DECAY_TIME, decayTime);
		AddHelper("Decay", "DECAY PROPERTY:\nThe Decay Time property sets the reverberation decay time.\nMinimum value sets typical small room with very dead surface.\n Maximum value sets typical large room with very live surfaces.", true);
		ImGui::SameLine(); ImGui::Text("    "); ImGui::SameLine();

		//DENSITY
		ImGui::SliderFloat("##ReverbDensity", &density, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_DENSITY, density);
		AddHelper("Density", "DENSITY PROPERTY:\nReverb Density property controls the coloration of the late reverb.\nLowering the value adds more coloration to the late reverb.", true);
		ImGui::SameLine(); ImGui::Text("   "); ImGui::SameLine();

		//ECHO DEPTH
		ImGui::SliderFloat("##ReverbEchoDepth", &echoDepth, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_ECHO_DEPTH, echoDepth);
		AddHelper("Echo Depth", "ECHO DEPTH PROPERTY:\nEcho Depth property introduces a cyclic echo in the reverberation decay, which will be noticeable with transient or percussive sounds.\nA larger value of Echo Depth will make this effect more prominent.", true);
		ImGui::SameLine();

		//REFLECT DELAY
		ImGui::SliderFloat("##ReverbReflectDelay", &reflectionDelay, 0.0f, 0.3f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_REFLECTIONS_DELAY, reflectionDelay);
		AddHelper("Rflc Delay", "REFLECT DELAY PROPERTY:\nThe Reflections Delay property is the amount of delay between the arrival time of the direct path from the source to the first reflection from the source.\nYou can reduce or increase Reflections Delay to simulate closer or more distant reflective surfaces\nand therefore control the perceived size of the room.", true);
		ImGui::Spacing();

		//DELAY TIME
		ImGui::SliderFloat("##ReverbDelayTime", &lateReverbDelay, 0.0f, 0.1f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_DELAY, lateReverbDelay);
		AddHelper("Delay Time", "DELAY TIME PROPERTY:\nThe Late Reverb Delay property defines the begin time of the late reverberation relative to the\ntime of the initial reflection (the first of the early reflections).\nReducing or increasing Late Reverb Delay is useful for simulating a smaller or larger room.", true);
		ImGui::SameLine();

		//DELAY GAIN
		ImGui::SliderFloat("Delay Gain", &lateReverbGain, 0.0f, 3.16f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_EAXREVERB_LATE_REVERB_GAIN, lateReverbGain);
		AddHelper("Delay Gain", "DELAY GAIN PROPERTY:\nThe Late Reverb Gain property controls the overall amount of later reverberation relative to the Gain property.\nNote that Late Reverb Gain and Decay Time are independent properties:\nIf you adjust Decay Time without changing Late Reverb Gain, the total intensity(the averaged square of the amplitude) of the late reverberation remains constant.", true);
		ImGui::SameLine();

		//MOD TIME
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
	Delay(ALint source, bool bypass) : Effect("Delay", AL_EFFECT_ECHO)
	{
		Generate(source, bypass);
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
		ImGui::Text("Delay"); ImGui::SameLine();
		if (ImGui::Checkbox("##BypassDl", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(150.0f);

		ImGui::SliderFloat("Wet       ", &feedback, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_FEEDBACK, feedback);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Spread    ", &spread, -1.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_SPREAD, spread);
		ImGui::Spacing();

		ImGui::SliderFloat("Time     ", &delaylr, 0.0f, 0.404f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_LRDELAY, delaylr);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Delay    ", &delay, 0.0f, 0.207f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_DELAY, delay);
		ImGui::Spacing();

		ImGui::SliderFloat("Damping  ", &damping, 0.0f, 0.99f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_ECHO_DAMPING, damping);

		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	float feedback = 0.5f, spread = -1.0f, delaylr = 0.1f,
		delay = 0.1f, damping = 0.5f;

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
		if (ImGui::Checkbox("##BypassD", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(150.0f);

		ImGui::SliderFloat("Wet       ", &gain, 0.01f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_GAIN, gain);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Edge      ", &edge, 0.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EDGE, edge);
		ImGui::Spacing();

		ImGui::SliderFloat("LP Cutoff ", &lowpassCutoff, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_LOWPASS_CUTOFF, lowpassCutoff);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("EQ Center ", &eqCenter, 80.0f, 24000.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_DISTORTION_EQCENTER, eqCenter);
		ImGui::Spacing();

		ImGui::SliderFloat("EQ Band W ", &eqBandWidth, 80.0f, 24000.0f, "%f");
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
	Flanger(ALint source, bool bypass) : Effect("Flanger", AL_EFFECT_FLANGER)
	{
		Generate(source, bypass);
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
		ImGui::Text("Flanger"); ImGui::SameLine();
		if (ImGui::Checkbox("##BypassF", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(150.0f);

		ImGui::SliderFloat("Depth     ", &depth, 0.01f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_DEPTH, depth);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Feedback  ", &feedback, -1.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_FEEDBACK, feedback);
		ImGui::Spacing();

		ImGui::SliderFloat("Delay     ", &delay, 0.0f, 0.004f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_DELAY, delay);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Rate      ", &rate, 0.0f, 10.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_RATE, rate);
		ImGui::Spacing();

		if (ImGui::Combo("          ", &waveformIndex, &waveform[0], waveform.size()))
		{
			ALint wave = AL_FLANGER_WAVEFORM_TRIANGLE;
			waveformIndex == 0 ? wave = AL_FLANGER_WAVEFORM_SINUSOID : wave = AL_FLANGER_WAVEFORM_TRIANGLE;
			SetEffectValue(AL_FLANGER_WAVEFORM, wave);
		}
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Phase     ", &phase, -180.0f, 180.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_FLANGER_PHASE, phase);

		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	std::vector<const char*> waveform = {"Sinoidal", "Triangular"};
	int waveformIndex = 0;

	float depth = 1.0f, feedback = -0.5f, delay = 0.002f,
		rate = 0.27f, phase = 0.0f;

};

class Chorus : public Effect
{
public:
	Chorus(ALint source, bool bypass) : Effect("Chorus", AL_EFFECT_CHORUS)
	{
		Generate(source, bypass);
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
		ImGui::Text("Chorus"); ImGui::SameLine();
		if (ImGui::Checkbox("##BypassC", &nobypass)) ToggleBypass(nobypass);

		ImGui::PushItemWidth(150.0f);

		ImGui::SliderFloat("Depth     ", &depth, 0.01f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_DEPTH, depth);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Feedback  ", &feedback, -1.0f, 1.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_FEEDBACK, feedback);
		ImGui::Spacing();

		ImGui::SliderFloat("Delay     ", &delay, 0.0f, 0.016f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_DELAY, delay);
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Rate      ", &rate, 0.0f, 10.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_RATE, rate);
		ImGui::Spacing();

		if (ImGui::Combo("          ", &waveformIndex, &waveform[0], waveform.size()))
		{
			ALint wave = AL_CHORUS_WAVEFORM_TRIANGLE;
			waveformIndex == 0 ? wave = AL_CHORUS_WAVEFORM_SINUSOID : wave = AL_CHORUS_WAVEFORM_TRIANGLE;
			SetEffectValue(AL_CHORUS_WAVEFORM, wave);
		}
		ImGui::SameLine(0.0f, 4.0f);
		ImGui::SliderFloat("Phase     ", &phase, -180.0f, 180.0f, "%f");
		if (ImGui::IsItemDeactivatedAfterEdit()) SetEffectValue(AL_CHORUS_PHASE, phase);

		ImGui::PopItemWidth();
	}

	void End()
	{

	}

private: // Methods

private: // Variables

	std::vector<const char*> waveform = { "Sinoidal", "Triangular" };
	int waveformIndex = 0;

	float depth = 0.1f, feedback = 0.25f, delay = 0.016f,
		rate = 1.1f, phase = 90.0f;

};

class AutoWah : public Effect
{
public:
	AutoWah(ALint source, bool bypass) : Effect("Auto Wah", AL_EFFECT_AUTOWAH)
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
	EQ(ALint source, bool bypass) : Effect("EQ", AL_EFFECT_EQUALIZER)
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
	Compressor(ALint source, bool bypass) : Effect("Compressor", AL_EFFECT_COMPRESSOR)
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
	FreqShift(ALint source, bool bypass) : Effect("Freq Shift", AL_EFFECT_FREQUENCY_SHIFTER)
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
	PitchShift(ALint source, bool bypass) : Effect("Pitch Shift", AL_EFFECT_PITCH_SHIFTER)
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
	RingMod(ALint source, bool bypass) : Effect("Ring Mod", AL_EFFECT_RING_MODULATOR)
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
	VocalMorph(ALint source, bool bypass) : Effect("Vocal Morph", AL_EFFECT_VOCAL_MORPHER)
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

