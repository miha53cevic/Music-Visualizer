//
// Author: Mihael Petricevic
// Contact: miha53cevic@gmail.com
// Date: 17.7.2019.
//
// FFT redux
//

#include "SFFT.h"

#include <iostream>

SFFT::SFFT()
{
}

void SFFT::hammingWindow(double* buffer)
{
	for (int i = 0; i < N; i++)
	{
		buffer[i] = 0.54 - (0.46 * cosf(2.0 * PI * (i / ((N - 1) * 1.0))));
	}
}

void SFFT::sfft(sf::SoundBuffer* buffer, sf::Sound* player)
{
	// Clear previous output if it exists
	m_output.clear();
	m_window.clear();
	m_magnitudes.clear();

	double window[N];
	hammingWindow(window);

	fftw_complex input[N];
	fftw_complex output[N];

	// Fill in the input
	for (int i = 0; i < N; i++)
	{
		int sampleIndex = i + (int)((player->getPlayingOffset().asMilliseconds() / 1000.0f) * buffer->getSampleRate());

		if (sampleIndex >= buffer->getSampleCount() - 1)
			return;

		// Window the sample / signal data
		input[i][REAL] = buffer->getSamples()[sampleIndex] * window[i];
		input[i][IMAG] = 0;

		m_window.push_back(input[i][REAL]);
	}

	// Perform FFT
	calculate_fft(input, output);

	for (int i = 0; i < N / 2 + 1; i++)
	{
		// Calculate magnitudes
		float magnitude = sqrtf(output[i][REAL] * output[i][REAL] + output[i][IMAG] * output[i][IMAG]);
		m_magnitudes.push_back(magnitude);

		m_output.push_back(std::make_pair(output[i][REAL], output[i][IMAG]));
	}
}

void SFFT::calculate_fft(fftw_complex* in, fftw_complex* out)
{
	fftw_plan p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);

	fftw_execute(p);

	// Memory cleanup
	fftw_destroy_plan(p);
	fftw_cleanup();
}

std::vector<float> SFFT::getOutput(int REALorIMAG)
{
	{
		std::vector<float> output;

		for (auto& bin : m_output)
		{
			if (REALorIMAG == REAL)
			{
				output.push_back(bin.first);
			}
			else if (REALorIMAG == IMAG)
			{
				output.push_back(bin.second);
			}
		}

		return output;
	}
}

float SFFT::getMagnitude(int index)
{
	return m_magnitudes.at(index);
}

float SFFT::getWindow(int index)
{
	return m_window.at(index);
}
