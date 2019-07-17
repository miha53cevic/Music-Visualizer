/*
// Author: Mihael Petricevic
// Contact: miha53cevic@gmail.com
//
// Description:
//		Simple SFML wrapper for easier usage
//
// Example usage:
//
//	#include "mihaSimpleSFML.h"
//
//	class demo : public mihaSimpleSFML
//	{
//	public:
//		demo() {}
//
//	private:
//
//	protected:
//		virtual bool OnUserCreate() override
//		{
//			return true;
//		}
//
//		virtual bool OnUserUpdate(sf::Time elapsed) override
//		{
//			return true;
//		}
//
//	};
//
//	int main()
//	{
//		demo app;
//		app.Construct(1280, 720, L"demo app");
//		app.Start();
//
//		return 0;
//	}
*/

#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <random>

// Random number generator in given range
float fRandom(float first, float second)
{
	std::random_device rd;
	std::default_random_engine generator(rd()); // rd() provides a random seed
	std::uniform_real_distribution<float> distribution(first, second);

	return distribution(generator);
}

int iRandom(int first, int second)
{
	std::random_device rd;
	std::default_random_engine generator(rd()); // rd() provides a random seed
	std::uniform_int_distribution<int> distribution(first, second);

	return distribution(generator);
}

// PI CONSTANT
const float PI = 3.141592f;

// Converts degress to radians
float toRadian(float deg)
{
	return deg * (PI / 180);
}

// Maps values from one range to another
float map(float s, float a1, float a2, float b1, float b2)
{
	return b1 + ((s - a1) * (b2 - b1)) / (a2 - a1);
}

class mihaSimpleSFML
{
public:
	mihaSimpleSFML()
	{
		m_sAppName = L"Default";
		m_nScreenWidth = 640;
		m_nScreenHeight = 480;

		m_bVSync = false;
		m_bDone = false;

		m_backgroundColor = sf::Color::Black;
	}
	~mihaSimpleSFML() { m_window.close(); }

	//Must override these
	virtual bool OnUserCreate() = 0;
	virtual bool OnUserUpdate(sf::Time elapsed) = 0;

	//Can override this
	virtual void Event(sf::Event e) {}

	int ScreenWidth() { return m_nScreenWidth; }
	int ScreenHeight() { return m_nScreenHeight; }

	void EnableVSync(bool l_vsync) { m_bVSync = l_vsync; m_window.setVerticalSyncEnabled(m_bVSync); }
	bool IsVSyncOn() { return m_bVSync; }

	sf::RenderWindow* getWindow() { return &m_window; }

	void Draw(sf::Drawable& l_drawable) { m_window.draw(l_drawable); }
	void Draw(sf::Drawable& l_drawable, sf::Transform& l_transform) { m_window.draw(l_drawable, l_transform); }
	void Draw(sf::VertexArray l_vertexArray) { m_window.draw(l_vertexArray); }

	void setBackgroundColor(sf::Color l_color) { m_backgroundColor = l_color; }

private:
	sf::RenderWindow m_window;

	int m_nScreenWidth;
	int m_nScreenHeight;

	std::wstring m_sAppName;

	sf::Color m_backgroundColor;

	bool m_bVSync;
	bool m_bDone;

	sf::Clock m_clock;
	sf::Time m_elapsed;

	void BeginDraw(sf::Color l_color) { m_window.clear(l_color); }
	void EndDraw() { m_window.display(); }

public:
	void Construct(int l_nScreenWidth, int l_nScreenHeight, std::wstring l_title)
	{
		m_nScreenWidth = l_nScreenWidth;
		m_nScreenHeight = l_nScreenHeight;

		m_sAppName = l_title;
	}

	void Start()
	{
		m_window.create(sf::VideoMode(m_nScreenWidth, m_nScreenHeight), m_sAppName);

		if (!OnUserCreate())
			m_bDone = true;

		//Main Game Loop
		while (!m_bDone)
		{
			sf::Event event;

			while (m_window.pollEvent(event))
			{
				if (event.type == sf::Event::Closed)
					m_bDone = true;

				Event(event);
			}

			BeginDraw(m_backgroundColor);

			if (!OnUserUpdate(m_elapsed))
				m_bDone = true;

			EndDraw();

			m_elapsed = m_clock.restart();
		}
	}
};