#include "Game.h"
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "CrazyTang", sf::Style::Default);

	ct::Game game{ window };

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
		}

		auto dt = clock.restart();

		window.clear();
		game.Update(dt.asSeconds());
		window.display();
	}
	return 0;
}