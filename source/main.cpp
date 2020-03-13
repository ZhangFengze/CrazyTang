#include <SFML/Graphics.hpp>
#include "Timer.h"

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "CrazyTang",
		sf::Style::Titlebar | sf::Style::Close);

	Timer timer;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		float dt = timer.Elapsed();
		timer.Reset();

		window.clear(sf::Color::Black);
		window.display();
	}

	return 0;
}