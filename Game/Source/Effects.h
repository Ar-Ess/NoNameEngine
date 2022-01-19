#ifndef __EFFECTS_H__
#define __EFFECTS_H__

#include "AudioSystem.h"

class Effect
{
public:
	~Effect() {}

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

public: // Variables

	bool selected = false;

protected:

	Effect(const char* xname, ALint ID)
	{
		name.clear();
		name = xname;
		id = ID;
	}

	void Generate()
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
	Reverb() : Effect("Reverb", AL_EFFECT_EAXREVERB)
	{
		Generate();
	}
	~Reverb() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
	}

	void End()
	{

	}

private: // Methods

private: // Variables

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
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

class Doppler : public Effect
{
public:
	Doppler() : Effect("Doppler", AL_DOPPLER_FACTOR)
	{
	}
	~Doppler() {}

	void Start()
	{
	}

	void Update()
	{
	}

	void Draw()
	{
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
	AutoWah() : Effect("AutoWah", AL_EFFECT_AUTOWAH)
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
	}

	void End()
	{

	}

private: // Methods

private: // Variables

};

#endif // !__EFFECTS_H__

