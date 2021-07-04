#pragma once
#include "common.h"
/*
A wave table oscillator class - jim Marshall 2021
*/
enum class WAVESHAPE {
	NONE,
	SINE,
	TRIANGLE,
	SAWTOOTH,
	SQUARE50,
	SQUARE25
};
class WaveTableOscillator
{
public:
    explicit  WaveTableOscillator(float f, WAVESHAPE s);
    explicit  WaveTableOscillator(float f, WAVESHAPE s, float amplitude, float phase_offset);
              ~WaveTableOscillator();
    float     getFreq() const { return freq; }
    bool      operator==(WaveTableOscillator w) {
		return w.freq == freq;
	}
    float     getNext();
    bool      active = false;
private:

	WAVESHAPE shape = WAVESHAPE::NONE;
	float     freq;
	float     left_phase = 0;
	float     right_phase = 0;
	float     phase = 0;
	float     amplitude = 0.25f;
	float     table[TABLE_SIZE];
private:
    double    lerp(double v0, double v1, double t) { return v0 + t * (v1 - v0); }
	void      initWaveTable();
	
	
};

