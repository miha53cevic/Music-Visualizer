#pragma once
#include <fftw3.h>
#include <vector>

#define REAL 0
#define IMAG 1
#define N 1024

const double PI = 3.141592653589793238460;

struct FFTW
{
	// FFTW complex to complex
	void fft(fftw_complex* in, fftw_complex* out)
	{
		fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

		fftw_execute(p); /* repeat as needed */

		fftw_destroy_plan(p);
		fftw_cleanup();
	}

	// FFTW real to complex
	void fft_real_to_complex(double* in, fftw_complex* out)
	{
		fftw_plan p = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

		fftw_execute(p); /* repeat as needed */

		fftw_destroy_plan(p);
		fftw_cleanup();
	}

	void visual_FFTW(sf::SoundBuffer* buffer, sf::Sound* sound)
	{
		vecMagnitudes.clear();

		fftw_complex input[N];
		fftw_complex output[N];

		for (int i = 0; i < N; i++)
		{
			input[i][REAL] = buffer->getSamples()[i + (int)(sound->getPlayingOffset().asSeconds() * buffer->getSampleRate())];
			input[i][IMAG] = 0;
		}

		// Hamming Window - bin
		// Smooths transition spikes of magnitues
		for (int i = 0; i < N; i++) {
			double multiplier = 0.5 * (1 - cosf(2 * PI*i / (N - 1)));
			input[i][REAL] *= multiplier;
		}

		// Run FFTW algorithm
		fft(input, output);

		// Calculate magnitudes
		for (int i = 0; i < N; i++)
		{
			// Pitagorin poucak
			int magnitude = sqrtf(output[i][REAL] * output[i][REAL] + output[i][IMAG] * output[i][IMAG]);

			vecMagnitudes.push_back(magnitude);
		}
	}

	void visual_FFTW_fromReal(sf::SoundBuffer* buffer, sf::Sound* sound)
	{
		vecMagnitudes.clear();

		 double input[N];
		 fftw_complex output[N / 2 + 1];

		for (int i = 0; i < N; i++)
		{
			input[i] = buffer->getSamples()[i + (int)(sound->getPlayingOffset().asSeconds() * buffer->getSampleRate())];
		}

		// Hamming Window - bin
		// Smooths transition spikes of magnitues
		for (int i = 0; i < N; i++) {
			double multiplier = 0.5 * (1 - cosf(2 * PI*i / (N - 1)));
			input[i] *= multiplier;
		}

		// Run FFTW algorithm
		fft_real_to_complex(input, output);

		// Calculate magnitudes
		for (int i = 0; i < N / 2 + 1; i++)
		{
			// Pitagorin poucak
			int magnitude = sqrtf(output[i][REAL] * output[i][REAL] + output[i][IMAG] * output[i][IMAG]);

			vecMagnitudes.push_back(magnitude);
		}
	}

	int getMagnitude(int pos)
	{
		return vecMagnitudes.at(pos);
	}

	std::vector<float> vecMagnitudes;
};