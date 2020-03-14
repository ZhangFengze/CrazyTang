#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

int main(int argc, char* argv[])
{
	sf::RenderWindow window(sf::VideoMode(800, 600, 32), "CrazyTang",
		sf::Style::Titlebar | sf::Style::Close);

	sf::Clock clock;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
		}

		auto dt = clock.restart();

		window.clear(sf::Color::Black);
		window.display();
	}

	return 0;
}