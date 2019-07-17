//
// Author: Mihael Petricevic
// Contact: miha53cevic@gmail.com
// Date: 17.7.2019.
//
// FFT redux
//

#include <iostream>
#include <fstream>

#include "mihaSimpleSFML.h"
#include "SFFT.h"

class demo : public mihaSimpleSFML
{
public:
	demo(std::string songPath) 
	{
		// Load song
		if (!buffer.loadFromFile(songPath))
		{
			// Error
		}
		else std::cout << "Loaded " + songPath + "\n";
	}

private:
	SFFT sfft;
	sf::SoundBuffer buffer;
	sf::Sound player;

	void DrawRectangle(sf::Vector2f pos, sf::Vector2i size, sf::Color c)
	{
		sf::RectangleShape shape;
		shape.setFillColor(c);
		shape.setPosition(pos);
		shape.setSize(sf::Vector2f(size.x, size.y));

		Draw(shape);
	}

protected:
	virtual bool OnUserCreate()
	{
		// Start the music
		player.setBuffer(buffer);
		player.play();

		player.setVolume(20.0f);

		std::cout << "Song sample rate is " << buffer.getSampleRate() << std::endl;

		return true;
	}

	virtual bool OnUserUpdate(sf::Time elapsed)
	{
		// Calculate SFFT
		sfft.sfft(&buffer, &player);

		sf::VertexArray window_wave;
		window_wave.setPrimitiveType(sf::PrimitiveType::LineStrip);

		for (int i = 0; i < N; i++)
		{
			// Plot only REAL part of the complex number
			float height = sfft.getWindow(i);

			// Scale factor
			height = map(height, -N, N, -ScreenHeight() / 8, ScreenHeight() / 8);

			float x = map(i, 0, N, 0, ScreenWidth() - 1);
			window_wave.append(sf::Vertex(sf::Vector2f(x, (ScreenHeight() / 4 - 1) - height), sf::Color::White));
		}

		sf::VertexArray wave;
		wave.setPrimitiveType((sf::PrimitiveType::LineStrip));

		// Find max magnitude
		float maxMagnitude = 0;

		for (int i = 0; i < N / 2 + 1; i++)
		{
			if (sfft.getMagnitude(i) > maxMagnitude)
				maxMagnitude = sfft.getMagnitude(i);
		}

		// Plot wave
		for (int x = 0; x < N / 2 + 1; x++)
		{
			float magnitude = sfft.getMagnitude(x);

			float xPos = map(x, 0, (N / 2) + 1, ScreenWidth() * 0.2, ScreenWidth() * 0.8);
			float yPos = map(magnitude / maxMagnitude, 0, 1, 0, ScreenHeight() / 2);
			//float yPos = 10 * logf(magnitude);

			wave.append(sf::Vertex(sf::Vector2f(xPos, (ScreenHeight() * 0.9) - yPos), sf::Color::Red));
		}

		Draw(wave);
		Draw(window_wave);

		return true;
	}

};

int main(int argc, char** argv)
{
	if (argc == 2)
	{
		// Load settings from config.cfg
		std::ifstream reader("settings.cfg");
		sf::Vector2u resolution(1280, 720);

		if (!reader.fail())
		{
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
			}

			reader.close();
		}
		else std::cout << "Could not find settings.cfg running default settings" << std::endl;

		demo Test(argv[1]);
		Test.Construct(resolution.x, resolution.y, L"SFFT Analyzer");
		Test.Start();
	}
	else
	{
		std::cout << "No music given!\nDrag and drop a .wav file on the exe\n";
		system("pause");
	}

	return 0;
}