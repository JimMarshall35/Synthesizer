#include "WaveTableOscillator.h"
#include <iostream>


float     WaveTableOscillator::sine[TABLE_SIZE];
float     WaveTableOscillator::triangle[TABLE_SIZE];
float     WaveTableOscillator::saw[TABLE_SIZE];
float     WaveTableOscillator::square50[TABLE_SIZE];
float     WaveTableOscillator::square25[TABLE_SIZE];

WaveTableOscillator::WaveTableOscillator(float f, WAVESHAPE s)
	: freq(f), shape(s)
{
	initWaveTable();
}

WaveTableOscillator::WaveTableOscillator(float f, WAVESHAPE s, float a, float p)
	: freq(f), shape(s), amplitude(a), phase(p)
{
	initWaveTable();
}


WaveTableOscillator::~WaveTableOscillator()
{
}


float WaveTableOscillator::getNext()
{
    if(!active){return 0.0f;}
	double   integer_part,fraction;
	/*
		1. split left and righ phase into integer and fraction parts
		2. get final sample value to output by interpolating between
		   the value in the wavetable at the index given by the integer part
		   and the next element of the table, using the fractional part
		   as t when interpolating
	*/
	fraction = modf(phase, &integer_part);

	phase += TABLE_SIZE * freq / SAMPLE_RATE;
	while (phase >= TABLE_SIZE) // while loop not if for high frequencies, might overshoot table with just 1 subtraction of table length
		phase -= TABLE_SIZE;
    return lerp(table[(int)integer_part], (integer_part + 1 >= TABLE_SIZE) ? table[0] : table[(int)integer_part + 1], fraction) * amplitude;
}

void WaveTableOscillator::initWaveTable()
{
	switch (shape)
	{
	case WAVESHAPE::SINE:
        table = sine;
		break;
	case WAVESHAPE::TRIANGLE:
        table = triangle;
		break;
	case WAVESHAPE::SAWTOOTH:
        table = saw;
		break;
	case WAVESHAPE::SQUARE50:
        table = square50;
		break;
	case WAVESHAPE::SQUARE25:
        table = square25;
		break;
	case WAVESHAPE::NONE:
	default:
		std::cerr << "wavetable oscillator passed invalid shape " << std::endl;
		break;
	}
	
}
void WaveTableOscillator::initWaveTables(){
    // saw
    for (int i = 0; i < TABLE_SIZE; i++){
        saw[i] = ((-1.0) + 2.0 * ((double)i / ((double)TABLE_SIZE))); // sawtooth
    }
    // sine
    for (int i = 0; i < TABLE_SIZE; i++){
        sine[i] = (float)sin(((double)i / (double)TABLE_SIZE) * M_PI * 2.0); // sine
    }
    // triangle
    for (int i = 0; i < TABLE_SIZE / 2; i++) {
        triangle[i] = (-1.0f + (float)i / ((float)TABLE_SIZE / 2.0f) * 2.0f);
    }
    for (int i = TABLE_SIZE / 2; i < TABLE_SIZE; i++) {
        triangle[i] = (1.0f - (float)(i - TABLE_SIZE / 2.0f) / ((float)TABLE_SIZE / 2.0f)*2.0f);
    }
    // square50
    for (int i = 0; i < TABLE_SIZE; i++){
        if (i < TABLE_SIZE / 2) {
            square50[i] = 1.0;
        }
        else {
            square50[i] = -1.0;
        }
    }
    // square25
    for (int i = 0; i < TABLE_SIZE; i++){
        if (i < TABLE_SIZE / 4) {
            square25[i] = 1.0;
        }
        else {
            square25[i] = -1.0;
        }
    }
}
