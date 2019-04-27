#pragma once
#include <complex>
#include <iostream>
#include <valarray>
#include <vector>

const double PI = 3.141592653589793238460;

typedef std::complex<double> Complex;
typedef std::valarray<Complex> CArray;

class FFT
{
public:
	FFT() {}

	void fft(CArray& x)
	{
		const size_t N = x.size();
		if (N <= 1) return;

		// divide
		CArray even = x[std::slice(0, N / 2, 2)];
		CArray  odd = x[std::slice(1, N / 2, 2)];

		// conquer
		fft(even);
		fft(odd);

		// combine
		for (size_t k = 0; k < N / 2; ++k)
		{
			Complex t = std::polar(1.0, -2 * PI * k / N) * odd[k];
			x[k] = even[k] + t;
			x[k + N / 2] = even[k] - t;
		}
	}

	void visualFFT(sf::SoundBuffer* buffer, sf::Sound* sound, int sampleRate)
	{
		magnitudes.clear();

		CArray bin(sampleRate);

		for (int i = 0; i < sampleRate; i++)
		{
			bin[i] = buffer->getSamples()[i + (int)(sound->getPlayingOffset().asSeconds() * buffer->getSampleRate())];
		}

		// Run FFT algorithm
		fft(bin);

		// Calculate magnitudes
		for (int i = 0; i < sampleRate; i++)
		{
			// Pitagorin poucak
			int magnitude = sqrtf(bin[i].real() * bin[i].real() + bin[i].imag() * bin[i].imag());
			//int magnitude = bin[i].real(); // In the middle going up and down
			magnitudes.push_back(magnitude);
		}
	}

	int getMagnitude(int pos)
	{
		return magnitudes.at(pos);
	}

private:
	std::vector<int> magnitudes;
};