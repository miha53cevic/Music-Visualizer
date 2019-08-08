//
// Author: Mihael Petricevic
// Contact: miha53cevic@gmail.com
// Originaly made: 17.7.2019.
// Updated: 8.8.2019.
//
// FFT redux
//

#pragma once
#include <SFML/Audio.hpp>
#include <fftw3.h>
#include <vector>

#ifndef N
#define N 16384
#endif // !N

#define REAL 0
#define IMAG 1

#define PI 3.141592f


class SFFT
{
public:
    SFFT();

    void sfft(sf::SoundBuffer* buffer, sf::Sound* player);

    float getMagnitude(int index);
    float getWindow(int index);

private:
    void hammingWindow(double* buffer);
    void calculate_fft(fftw_complex* input, fftw_complex* output);

    std::vector<float> m_window;
    std::vector<float> m_magnitudes;
};