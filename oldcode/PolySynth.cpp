#include "PolySynth.h"
#include <cstring>

PolySynth::PolySynth()
{
	test_setup_oscillators();
}


PolySynth::~PolySynth()
{
}
bool PolySynth::open(PaDeviceIndex index)
{
	PaStreamParameters outputParameters;

	outputParameters.device = index;
	if (outputParameters.device == paNoDevice) {
		return false;
	}

	const PaDeviceInfo* pInfo = Pa_GetDeviceInfo(index);
	if (pInfo != 0)
	{
		printf("Output device name: '%s'\r", pInfo->name);
	}

	outputParameters.channelCount = 2;       /* stereo output */
	outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
	outputParameters.suggestedLatency = Pa_GetDeviceInfo(outputParameters.device)->defaultLowOutputLatency;
	outputParameters.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(
		&stream,
		NULL, /* no input */
		&outputParameters,
		SAMPLE_RATE,
		paFramesPerBufferUnspecified,
		paClipOff,
		&PolySynth::paCallback,
		this            /* Using 'this' for userData so we can cast to PolySynth* in paCallback method */
	);

	if (err != paNoError)
	{
		/* Failed to open stream to device !!! */
		return false;
	}

	err = Pa_SetStreamFinishedCallback(stream, &PolySynth::paStreamFinished);

	if (err != paNoError)
	{
		Pa_CloseStream(stream);
		stream = 0;

		return false;
	}

	return true;
}

bool PolySynth::close()
{
	if (stream == 0)
		return false;

	PaError err = Pa_CloseStream(stream);
	stream = 0;

	return (err == paNoError);
}

bool PolySynth::start()
{
	if (stream == 0)
		return false;

	PaError err = Pa_StartStream(stream);

	return (err == paNoError);
}

bool PolySynth::stop()
{
	if (stream == 0)
		return false;

	PaError err = Pa_StopStream(stream);

	return (err == paNoError);
}

void PolySynth::startTone(float freq, int key)
{
	for (auto& note : notes) {
		if (note.getKey() == key) {
			note.restart();
			return;
		}
	}
	std::lock_guard<std::mutex>lock(m);
	notes.push_back(PolySynthNote(this,freq,key));
}

void PolySynth::stopTone(int key)
{
	for (auto& note : notes)
	{
		if (note.getKey() == key) {
			//std::lock_guard<std::mutex>lock(m);
			//notes.remove(note);
			note.release();
			return;
		}
	}
}

void PolySynth::cleanupNotes()
{
	for (auto& note : notes) {
		if (note.getADSRPhase() == ADSR::DONE) {
			notes2Remove.push(&note);
		}
	}
	while (notes2Remove.size() > 0) {
		removeNote(notes2Remove.front());
		notes2Remove.pop();
	}
}

void PolySynth::removeNote(PolySynthNote * n)
{
	std::lock_guard<std::mutex>lock(m);
	notes.remove(*n);
}

void PolySynth::test_setup_oscillators()
{
	OscillatorDescription d, d2;
	d.amplitude = 0.15;
	d.shape     = WAVESHAPE::SINE;
	oscillators_state.push_back(d);

	d2.amplitude = 0.05;
	d2.shape     = WAVESHAPE::SQUARE25;
	//d2.phase_offset = TABLE_SIZE / 2;
	oscillators_state.push_back(d2);

    adsr_state.attack  = 0.000001;
    adsr_state.decay   = 0.000001;
    adsr_state.sustain = 0.000001;
    adsr_state.release = 0.000001;
}

#include "timer.h"
int PolySynth::paCallbackMethod(const void * inputBuffer, void * outputBuffer, unsigned long framesPerBuffer, const PaStreamCallbackTimeInfo * timeInfo, PaStreamCallbackFlags statusFlags)
{
	(void)timeInfo; /* Prevent unused variable warnings. */
	(void)statusFlags;
	(void)inputBuffer;

	 

	float* out = (float*)outputBuffer;

	memset(out, 0, framesPerBuffer*NUM_CHANNELS * sizeof(float));
	
	float max = 0;
	

	for (size_t i = 0; i < framesPerBuffer; i++)
	{
		float l_val = 0;
		float r_val = 0;
		std::lock_guard<std::mutex>lock(m);
		for (auto& note : notes)
		{
			float next = note.getNext();
			l_val += next;
			r_val += next;

			
		}
		*out++ = l_val;
		*out++ = r_val;
	}


	return paContinue;
}

