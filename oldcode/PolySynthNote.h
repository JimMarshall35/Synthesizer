#pragma once
#include <vector>
#include "WaveTableOscillator.h"
class PolySynth;
struct ADSR_Settings {
	double attack;
	double decay;
	double sustain;
	double release;
};
enum class ADSR {
	A,
	D,
	S,
	R,
	DONE
};
class PolySynthNote {
public:
	PolySynthNote(PolySynth* synth, float freq, int key);
	~PolySynthNote();
	float getNext();
	int  getKey() const { return key; }
	bool operator==(PolySynthNote w) {
		return w.key == key;
	}
	void release() { 

		adsr_phase = ADSR::R; 
	}
	ADSR getADSRPhase() { return adsr_phase; }
    void restart() { gain = 0.0f, adsr_phase = ADSR::A, samples = 0, samples_sustained = 0, samples_attacked=0,samples_decayed=0; }
private:
	
	float                            gain = 0.0f;
	float                            lastgain = 0.0f;
	PolySynth*                       synth;
	std::vector<WaveTableOscillator> oscillators;
    int                              key;                // Qts cross platformr code for the key that was pressed
	long unsigned int                samples = 0;
	ADSR                             adsr_phase = ADSR::A;
	ADSR_Settings                    adsr_settings;
    long unsigned int                samples_sustained = 0;
    long unsigned int                samples_attacked = 0;
    long unsigned int                samples_decayed = 0;
private:
	void                             applyADSR();
	

};
