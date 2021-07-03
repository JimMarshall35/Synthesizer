#pragma once
#include "common.h"
#include <cmath>


/*
port audio example file modified - a single monophonic 
sine wave capable of playing different frequencies
*/
class SineWaveTable
{
public:
	SineWaveTable() : stream(0), left_phase(0), right_phase(0)
	{
		/* initialise sinusoidal wavetable */
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			sine[i] = (float)sin(((double)i / (double)TABLE_SIZE) * M_PI * 2.);
		}

		sprintf_s(message, "No Message");
	}

	bool open(PaDeviceIndex index)
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
			paClipOff,      /* we won't output out of range samples so don't bother clipping them */
			&SineWaveTable::paCallback,
			this            /* Using 'this' for userData so we can cast to Sine* in paCallback method */
		);

		if (err != paNoError)
		{
			/* Failed to open stream to device !!! */
			return false;
		}

		err = Pa_SetStreamFinishedCallback(stream, &SineWaveTable::paStreamFinished);

		if (err != paNoError)
		{
			Pa_CloseStream(stream);
			stream = 0;

			return false;
		}

		return true;
	}

	bool close()
	{
		if (stream == 0)
			return false;

		PaError err = Pa_CloseStream(stream);
		stream = 0;

		return (err == paNoError);
	}


	bool start()
	{
		playing = true;
		if (stream == 0)
			return false;

		PaError err = Pa_StartStream(stream);

		return (err == paNoError);
	}

	bool stop()
	{
		playing = false;
		if (stream == 0)
			return false;

		PaError err = Pa_StopStream(stream);

		return (err == paNoError);
	}
	bool playing = false;
	bool getPlaying() { return playing; }
	float freq;
private:
	float lerp(float v0, float v1, float t) {
		return v0 + t * (v1 - v0);
	}
	/* The instance callback, where we have access to every method/variable in object of class Sine */
	int paCallbackMethod(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags)
	{
		float *out = (float*)outputBuffer;
		if (!playing) {
			memset(out, 0, framesPerBuffer*NUM_CHANNELS*sizeof(float));
			return paContinue;
		}
		unsigned long i;

		(void)timeInfo; /* Prevent unused variable warnings. */
		(void)statusFlags;
		(void)inputBuffer;


		for (i = 0; i < framesPerBuffer; i++)
		{
			double   integer_L, integer_R, fraction_L, fraction_R;
			/*
				1. split left and righ phase into integer and fraction parts
				2. get final sample value to output by interpolating between 
				   the value in the wavetable at the index given by the integer part
				   and the next element of the table, using the fractional part
				   as t when interpolating
			*/
			fraction_L = modf(left_phase, &integer_L);
			fraction_R = modf(right_phase, &integer_R);

			/* left */
			*out++ = lerp(sine[(int)integer_L], (integer_L + 1 >= TABLE_SIZE) ? sine[0] : sine[(int)integer_L + 1], fraction_L);// mega line 
			/* right */
			*out++ = lerp(sine[(int)integer_R], (integer_R + 1 >= TABLE_SIZE) ? sine[0] : sine[(int)integer_R + 1], fraction_R); // to big to fail

			left_phase += TABLE_SIZE * freq / SAMPLE_RATE;
			while(left_phase >= TABLE_SIZE) // while loop not if for high frequencies, might overshoot table with just 1 subtraction of table length
				left_phase -= TABLE_SIZE;
			right_phase += TABLE_SIZE * freq / SAMPLE_RATE; 
			while (right_phase >= TABLE_SIZE) 
				right_phase -= TABLE_SIZE;

		}

		return paContinue;

	}

	/* This routine will be called by the PortAudio engine when audio is needed.
	** It may called at interrupt level on some machines so don't do anything
	** that could mess up the system like calling malloc() or free().
	*/
	static int paCallback(const void *inputBuffer, void *outputBuffer,
		unsigned long framesPerBuffer,
		const PaStreamCallbackTimeInfo* timeInfo,
		PaStreamCallbackFlags statusFlags,
		void *userData)
	{
		/* Here we cast userData to Sine* type so we can call the instance method paCallbackMethod, we can do that since
		   we called Pa_OpenStream with 'this' for userData */
		return ((SineWaveTable*)userData)->paCallbackMethod(inputBuffer, outputBuffer,
			framesPerBuffer,
			timeInfo,
			statusFlags);
	}


	void paStreamFinishedMethod()
	{
		//printf("Stream Completed: %s\n", message);
	}

	/*
	 * This routine is called by portaudio when playback is done.
	 */
	static void paStreamFinished(void* userData)
	{
		return ((SineWaveTable*)userData)->paStreamFinishedMethod();
	}

	PaStream *stream;
	
	float sine[TABLE_SIZE];
	float left_phase=0;
	float right_phase=0;
	char  message[20];
};