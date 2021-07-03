#pragma once
#include <list>
#include "common.h"
#include "WaveTableOscillator.h"
#include <mutex>
#include <vector>
#include "PolySynthNote.h"
#include <queue>
/*
a polyphonic synth class - Jim Marshall

- owns and manages a list of wavetable oscillators, public interface: startTone and StopTOne
- sets up port audio stream for itself with open, close, start, stop 
*/

class PolySynthNote;
struct OscillatorDescription {
	WAVESHAPE shape;
	float     amplitude;
	float     phase_offset = 0;
};
class PolySynth
{
public:
	friend class PolySynthNote;
	PolySynth();
	~PolySynth();
	bool open(PaDeviceIndex index);
	bool close();
	bool start();
	bool stop();
	void startTone(float freq, int key);
	void stopTone(int key);
	void clearOscillators() { notes.clear(); }
	OscillatorDescription* getOscillatorsState(size_t& size) { 
		size = oscillators_state.size();
		return &oscillators_state[0]; 
	}
	ADSR_Settings getADSR_State() { return adsr_state; }
	void cleanupNotes();
    ADSR_Settings                      adsr_state;
private:
	std::queue<PolySynthNote*>         notes2Remove;
	void                               removeNote(PolySynthNote* n);

	void                               test_setup_oscillators();
	std::vector<OscillatorDescription> oscillators_state;
	std::list<PolySynthNote>           notes;
	void						       paStreamFinishedMethod() {}
	static void                        paStreamFinished(void* userData) { return ((PolySynth*)userData)->paStreamFinishedMethod(); }
	PaStream*                          stream;
	std::mutex                         m; // see comment in paCallbackMethod
private:

	int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags);
		
	

	static int paCallback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData)
	{
		return ((PolySynth*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
			framesPerBuffer,
			timeInfo,
			statusFlags);
	}
	
};
