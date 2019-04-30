/*
// Made by: Mihael Petrièeviæ
// Date: 27.4.2019 ---> 30.4.2019.
*/

#include "mihaSimpleSFML.h"

#include <SFML/Audio.hpp>
#include <fstream>

#include "FFT.h"

bool vsync = false;

class FFTWApp : public mihaSimpleSFML
{
public:
	FFTWApp(std::string path)
		: songPath(path)
	{}

private:
	sf::SoundBuffer buffer;
	sf::Sound sound;

	FFTW fft;

	std::string songPath;

	float maxFreq = 1.0f;

protected:
	bool virtual OnUserCreate()
	{
		// Song path when .wav file dropped on .exe
		printf("%s\n", songPath.c_str());

		// V-Sync
		EnableVSync(vsync);
		printf("V-Sync is %s", (vsync) ? "enabled" : "disabled");

		// Load music
		if (!buffer.loadFromFile(songPath))
		{
			// Error
		}

		// Play music
		sound.setBuffer(buffer);
		sound.setVolume(50);
		sound.play();

		return true;
	}

	bool virtual OnUserUpdate(sf::Time elapsed)
	{
		fft.visual_FFTW(&buffer, &sound);

		sf::VertexArray sinWave;
		sinWave.setPrimitiveType(sf::LineStrip);

		sf::VertexArray logWave;
		logWave.setPrimitiveType(sf::LineStrip);

		// Signal nakon hamming windowa
		for (int k = 0; k < N; k++)
		{
			// Plot only REAL part of the complex number
			float height = fft.getInput(k, REAL);

			// Scale factor
			height = map(height, -N, N, -ScreenHeight() / 8, ScreenHeight() / 8);

			float x = map(k, 0, N, 0, ScreenWidth() - 1);
			sinWave.append(sf::Vertex(sf::Vector2f(x, (ScreenHeight() / 4 - 1) - height), sf::Color::White));
		}

		// Draw Magnitude power
		for (int i = 0; i < fft.vecMagnitudes.size(); i++)
		{
			float height = fft.getMagnitude(i);

			// Get max magnitude
			if (maxFreq < height) maxFreq = height;

			// Scale factor
			height = map(height, 0, maxFreq, 0, ScreenHeight() / 2);
			
			float x = map(i, 0, fft.vecMagnitudes.size() - 1, ScreenWidth() / 10, ScreenWidth() - ScreenWidth() / 10 - 1);
			logWave.append(sf::Vertex(sf::Vector2f(x, (ScreenHeight() - ScreenHeight() / 10) - height), sf::Color::Red));
		}

		// Draw both waves
		Draw(sinWave);
		Draw(logWave);
		
		// If song ended exit
		if (sound.getPlayingOffset() >= buffer.getDuration())
		{
			return false;
		}

		return true;
	}
};

int main(int argc, char* argv[])
{
	if (argc != 2)
		return -1;
	
	// Setting resolution through settings.txt file
	std::ifstream reader("settings.txt");
	sf::Vector2u resolution(640, 720);

	int i = 0;
	std::string line;
	while (std::getline(reader, line))
	{
		if (line == "resolution")
		{
			int val;
			reader >> val;
			resolution.x = val;
			reader >> val;
			resolution.y = val;
		}
		else if (line == "V-Sync")
		{
			std::string val;
			reader >> val;
			if (val == "true")
				vsync = true;
		}
	}

	FFTWApp app(argv[1]);
	app.Construct(resolution.x, resolution.y, L"FFTW Visualization");
	app.Start();

	return 0;
}