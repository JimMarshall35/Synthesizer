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
        WaveTableOscillator w = WaveTableOscillator(freq + d.detune, d.shape, d.amplitude, d.phase_offset);
        if(d.active){
             w.active = true;
        }
        oscillators.push_back(w);

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
    double t, seconds_until_release;
	samples++;
	double seconds_passed = (double)samples / (double)SAMPLE_RATE;
	switch (adsr_phase)
	{
	case ADSR::A:
        samples_attacked++;
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
        samples_decayed++;
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
        lastgain = gain;
		break;
	case ADSR::R:
        seconds_until_release = (double)(samples_sustained+samples_attacked+samples_decayed) / (double)SAMPLE_RATE;

        t = (seconds_passed - (seconds_until_release)) / adsr_settings.release;
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
