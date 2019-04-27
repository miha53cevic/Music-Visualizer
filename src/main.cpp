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
		if (!buffer.loadFromFile("Eminem - Mockingbird [HQ].wav"))
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
		fft.visualFFT(&buffer, &sound, 1024);
		
		int k = 0;
		while (k < (int)(ScreenWidth() / rectSize))
		{
			// Get amplitude of signal through pitagorin poucak
			int height = fft.getMagnitude(k) / 1024;

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(rectSize, height));
			rect.setPosition(k * rectSize, (ScreenHeight() - 1) - height);

			Draw(rect);

			k++;
		}

		// In the middle going up and down
		/*int k = 0;
		while (k < (int)(ScreenWidth() / rectSize))
		{
			// Get amplitude of signal through pitagorin poucak
			int height = fft.getMagnitude(k) / 1024;

			sf::RectangleShape rect;
			rect.setSize(sf::Vector2f(rectSize, height));
			rect.setPosition(k * rectSize, (ScreenHeight() / 2 - 1) - height);

			Draw(rect);

			k++;
		}*/

		// Circle pulsating
		float average = 0;
		for (int i = 0; i < 1024; i++)
		{
			average += fft.getMagnitude(i);
		}
		average /= 1024;
		average /= 1024;

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