#include "WaveTableOscillator.h"
#include <iostream>



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
	return lerp(table[(int)integer_part], (integer_part + 1 >= TABLE_SIZE) ? table[0] : table[(int)integer_part + 1], fraction);
}

void WaveTableOscillator::initWaveTable()
{
	auto genSawTooth = [this]()mutable {
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			table[i] = ((-1.0) + 2.0 * ((double)i / ((double)TABLE_SIZE)))* amplitude; // sawtooth
		}
	};
	auto genSine = [this]()mutable {
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			table[i] = (float)sin(((double)i / (double)TABLE_SIZE) * M_PI * 2.0) * amplitude; // sine
		}
	};
	auto genTriangle = [this]()mutable {
		for (int i = 0; i < TABLE_SIZE / 2; i++) {
			table[i] = (-1.0f + (float)i / ((float)TABLE_SIZE / 2.0f) * 2.0f)* amplitude;
		}
		for (int i = TABLE_SIZE / 2; i < TABLE_SIZE; i++) {
			table[i] = (1.0f - (float)(i - TABLE_SIZE / 2.0f) / ((float)TABLE_SIZE / 2.0f)*2.0f) * amplitude;
		}
	};
	auto genSquare50 = [this]()mutable {
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			if (i < TABLE_SIZE / 2) {
				table[i] = 1.0* amplitude;
			}
			else {
				table[i] = -1.0* amplitude;
			}
		}
	};
	auto genSquare25 = [this]()mutable {
		for (int i = 0; i < TABLE_SIZE; i++)
		{
			if (i < TABLE_SIZE / 4) {
				table[i] = 1.0* amplitude;
			}
			else {
				table[i] = -1.0* amplitude;
			}
		}
	};
	switch (shape)
	{
	case WAVESHAPE::SINE:
		genSine();
		break;
	case WAVESHAPE::TRIANGLE:
		genTriangle();
		break;
	case WAVESHAPE::SAWTOOTH:
		genSawTooth();
		break;
	case WAVESHAPE::SQUARE50:
		genSquare50();
		break;
	case WAVESHAPE::SQUARE25:
		genSquare25();
		break;
	case WAVESHAPE::NONE:
	default:
		std::cerr << "wavetable oscillator passed invalid shape " << std::endl;
		break;
	}
	
}
