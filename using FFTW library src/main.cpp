#include "mihaSimpleSFML.h"

#include <SFML/Audio.hpp>
#include <iostream>

#include "FFT.h"

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

	const int rectSize = 4;

protected:
	bool virtual OnUserCreate()
	{
		// Song path when .wav file dropped on .exe
		std::cout << songPath;

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

		int k = 0;
		while (k < (int)(ScreenWidth() / rectSize))
		{
			// Get amplitude of signal through pitagorin poucak
			int height = fft.getMagnitude(k) / N;
			int height2 = 20 * logf(fft.getMagnitude(k)); // Get magnitude_DB

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(rectSize, height));
			rect.setPosition(k * rectSize, (ScreenHeight() / 2 - 1) - height);

			Draw(rect);

			rect.setSize(sf::Vector2f(rectSize, height2));
			rect.setPosition(k * rectSize, (ScreenHeight() - 1) - height2);

			Draw(rect);

			k++;
		}

		// Circle pulsating
		float average = 0;
		for (int i = 0; i < N; i++)
		{
			average += fft.getMagnitude(i);
		}
		average /= N;
		average /= N;

		sf::CircleShape circle;
		circle.setRadius(average);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(ScreenWidth() / 2, ScreenHeight() / 2);

		Draw(circle);

		// If song ended exit
		if (sound.getStatus() == sound.Stopped)
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
	
	FFTWApp app(argv[1]);
	app.Construct(1280, 720, L"FFTW Visualization");
	app.Start();

	return 0;
}