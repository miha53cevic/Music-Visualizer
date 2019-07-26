/*
// Made by: Mihael Petrièeviæ
// Date: 27.4.2019 ---> 30.4.2019.
*/

#pragma once
#include <fftw3.h>
#include <vector>

#define REAL 0
#define IMAG 1
#define N 16384 // 2^14

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

	void visual_FFTW(sf::SoundBuffer* buffer, sf::Sound* sound)
	{
		vecMagnitudes.clear();
		vecOutput.clear();
		vecInput.clear();

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

			vecInput.push_back(std::make_pair(input[i][REAL], input[i][IMAG]));
		}

		// Run FFTW algorithm
		fft(input, output);

		// Calculate magnitudes
		for (int i = 0; i < N / 2 - 1; i++)	// Last useful bin in practical apps is at N / 2 - 1
		{
			// Pitagorin poucak
			float magnitude = sqrtf(output[i][REAL] * output[i][REAL] + output[i][IMAG] * output[i][IMAG]);

			vecMagnitudes.push_back(magnitude);
			vecOutput.push_back(std::make_pair(output[i][REAL], output[i][IMAG]));
		}
	}

	float getMagnitude(int pos)
	{
		return vecMagnitudes.at(pos);
	}

	float getOutput(int pos, int RealOrImag)
	{
		return (RealOrImag == REAL) ? vecOutput.at(pos).first : vecOutput.at(pos).second;
	}

	float getInput(int pos, int RealOrImag)
	{
		return (RealOrImag == REAL) ? vecInput.at(pos).first : vecInput.at(pos).second;
	}

	std::vector<float> vecMagnitudes;
	std::vector<std::pair<float, float>> vecOutput;
	std::vector<std::pair<float, float>> vecInput;
};