#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "mihaSimpleSFML.h"

#include "fft.h"

class MusicVisualizer : public mihaSimpleSFML
{
public:
	MusicVisualizer() {}

private:
	sf::SoundBuffer buffer;
	sf::Sound sound;
	FFT fft;

	const int rectSize = 4;

protected:
	bool virtual OnUserCreate()
	{
		// Load music
		if (!buffer.loadFromFile("Imagine Dragons Believer (8D AUDIO).wav"))
		{
			// Error
		}

		// Play music
		sound.setBuffer(buffer);
		sound.setVolume(30);
		sound.play();
		
		return true;
	}

	bool virtual OnUserUpdate(sf::Time elapsed)
	{
		const int sampleRate = 1024;

		fft.visualFFT(&buffer, &sound, sampleRate);
		
		int k = 0;
		while (k < (int)(ScreenWidth() / rectSize))
		{
			// Get amplitude of signal through pitagorin poucak
			int height = fft.getMagnitude(k) / sampleRate;

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(rectSize, height));
			rect.setPosition(k * rectSize, (ScreenHeight() - 1) - height);

			Draw(rect);

			k++;
		}

		// Circle pulsating
		float average = 0;
		for (int i = 0; i < sampleRate; i++)
		{
			average += fft.getMagnitude(i);
		}
		average /= sampleRate;
		average /= sampleRate;

		sf::CircleShape circle;
		circle.setRadius(average);
		circle.setOrigin(circle.getRadius(), circle.getRadius());
		circle.setPosition(ScreenWidth() / 2, ScreenHeight() / 2);
		
		Draw(circle);

		return true;
	}
};

int main()
{
	MusicVisualizer app;
	app.Construct(1280, 720, L"Music Visualizer");
	app.Start();

	return 0;
}