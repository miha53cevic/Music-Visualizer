//
// Author: Mihael Petricevic
// Contact: miha53cevic@gmail.com
// Originaly made: 17.7.2019.
// Updated: 8.8.2019.
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
    demo(std::string songPath, unsigned int QUAD_SIZE)
    {
        // Load song
        if (!buffer.loadFromFile(songPath))
        {
            // Error
        }
        else std::cout << "Loaded " + songPath + "\n";

        this->QUAD_SIZE = QUAD_SIZE;
    }

private:
    SFFT sfft;
    sf::SoundBuffer buffer;
    sf::Sound player;

    unsigned int QUAD_SIZE;

    void DrawRectangle(sf::Vector2f pos, sf::Vector2i size, sf::Color c)
    {
        sf::RectangleShape shape;
        shape.setFillColor(c);
        shape.setPosition(pos);
        shape.setSize(sf::Vector2f(size.x, size.y));
        shape.setOutlineColor(sf::Color::Black);
        shape.setOutlineThickness(1);

        Draw(shape);
    }

protected:
    virtual void Event(sf::Event e) override
    {
        if (e.type == sf::Event::KeyPressed)
        {
            if (e.key.code == sf::Keyboard::Key::Up)
            {
                player.setVolume(player.getVolume() + 10);
            }
            else if (e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::Key::Down)
            {
                player.setVolume(player.getVolume() - 10);
            }

            // Set maximum and minimum
            if      (player.getVolume() > 100)  player.setVolume(100);
            else if (player.getVolume() < 0)    player.setVolume(0);

            std::cout << "Changed volume to " << player.getVolume() << std::endl;
        }
    }

protected:
    virtual bool OnUserCreate() override
    {
        // Enable VSYNC
        EnableVSync(true);

        // Start the music
        player.setBuffer(buffer);
        player.play();

        // Set volume
        player.setVolume(20.0f);

        // Display song info
        std::cout << "Song sample rate is " << buffer.getSampleRate() << std::endl;
        std::cout << "Song channel count is " << buffer.getChannelCount() << std::endl;

        std::cout << "Controls:\n\tUP ARROW --> Volume UP\n\tDOWN ARROW --> Volume DOWN" << std::endl;

        return true;
    }

    virtual bool OnUserUpdate(sf::Time elapsed) override
    {
        // Calculate SFFT
        sfft.sfft(&buffer, &player);

        // If song is over exit
        if (player.getStatus() == sf::Sound::Status::Stopped)
            return false;

        sf::VertexArray wave;
        wave.setPrimitiveType((sf::PrimitiveType::LineStrip));

        // Plot wave
        // nqyuist is the N/2th freq
        // freq = i * sampleRate / N
        float nqyuist = (N / 2) * (buffer.getChannelCount() * buffer.getSampleRate()) / N;

        // Where to see the freq_bin values
        // https://www.teachmeaudio.com/mixing/techniques/audio-spectrum/
        std::vector<float> freq_bin = { 20, 60, 250, 500/*, 2000, 4000, 6000, nqyuist*/ };
        std::vector<float> peakmaxArray/*(freq_bin.size() - 1, -INFINITY)*/;

        for (int x = 0; x < N / 2 + 1; x++)
        {
            float freq = x * (buffer.getChannelCount() * buffer.getSampleRate()) / N;
            float magnitude = sfft.getMagnitude(x);

            float xPos = map(x, 0, (N / 2) + 1, ScreenWidth() * 0.2, ScreenWidth() * 0.8);
            float yPos = magnitude * 0.000005f;

            for (int i = 0; i < freq_bin.size() - 1; i++)
            {
                if ((freq > freq_bin[i]) && (freq <= freq_bin[i + 1])) {
                    /*if (magnitude > peakmaxArray[i]) {
                        peakmaxArray[i] = magnitude;
                    }*/
                    peakmaxArray.push_back(magnitude);
                }
            }

            wave.append(sf::Vertex(sf::Vector2f(xPos, (ScreenHeight() * 0.9) - yPos), sf::Color::Red));
        }

        for (int i = 0; i < peakmaxArray.size(); i++)
        {
            float x = i * QUAD_SIZE + ((ScreenWidth() / 2) - (peakmaxArray.size() * QUAD_SIZE / 2));
            float y = ScreenHeight() / 3;

            float height = peakmaxArray[i] * 0.0000025f;
            
            DrawRectangle({ x, y }, { (int)QUAD_SIZE, (int)(-height) }, sf::Color::Blue);
        }

        Draw(wave);

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
        unsigned int QUAD_SIZE = 8;

        if (!reader.fail())
        {
            int i = 0;
            std::string line;
            while (std::getline(reader, line))
            {
                if (line == "resolution")
                {
                    reader >> resolution.x;
                    reader >> resolution.y;
                }
                else if (line == "QUAD_SIZE")
                {
                    reader >> QUAD_SIZE;
                }
            }

            reader.close();
        }
        else std::cout << "Could not find settings.cfg running default settings" << std::endl;
        
        demo Test(argv[1], QUAD_SIZE);
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