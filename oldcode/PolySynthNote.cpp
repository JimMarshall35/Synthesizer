#include "PolySynthNote.h"
#include "PolySynth.h"
double     lerp(double v0, double v1, double t) { return v0 + t * (v1 - v0); }
PolySynthNote::PolySynthNote(PolySynth * synth, float freq, int k)
	: key(k)
{
	size_t num_oscillators = 0;
	OscillatorDescription* descriptions = synth->getOscillatorsState(num_oscillators);
	for (size_t i = 0; i < num_oscillators; i++) {
		OscillatorDescription d = descriptions[i];
		oscillators.push_back(WaveTableOscillator(freq, d.shape, d.amplitude, d.phase_offset));
	}
	adsr_settings = synth->getADSR_State();
}

PolySynthNote::~PolySynthNote()
{
}

float PolySynthNote::getNext()
{
	float val = 0;
	for (size_t i = 0; i < oscillators.size(); i++) {
		WaveTableOscillator* osc = &oscillators[i];
		val += osc->getNext();
	}
	val *= gain;
	applyADSR();
	return val;
}

void PolySynthNote::applyADSR()
{
	double t, seconds_sustained;
	samples++;
	double seconds_passed = (double)samples / (double)SAMPLE_RATE;
	switch (adsr_phase)
	{
	case ADSR::A:
		if (seconds_passed > adsr_settings.attack) {
			adsr_phase = ADSR::D;
			gain = 1.0;
			lastgain = gain;
			break;
		}
		gain = seconds_passed / adsr_settings.attack;
		lastgain = gain;
		break;
	case ADSR::D:
		if (seconds_passed > adsr_settings.attack + adsr_settings.decay) {
			adsr_phase = ADSR::S;
			gain = adsr_settings.sustain;
			lastgain = gain;
			break;
		}
		t = (seconds_passed - adsr_settings.attack) / adsr_settings.decay;
		gain = lerp(1.0, adsr_settings.sustain, t);
		lastgain = gain;
		break;
    case ADSR::S:
		samples_sustained++;
		break;
	case ADSR::R:
		seconds_sustained = (double)samples_sustained / (double)SAMPLE_RATE;
		t = (seconds_passed - (seconds_sustained + adsr_settings.attack + adsr_settings.decay)) / adsr_settings.release;
		if (t >= 1) {
			adsr_phase = ADSR::DONE;
			break;
		}
		gain = lerp(lastgain, 0, t);
		break;
	default:
		break;
	}
}
