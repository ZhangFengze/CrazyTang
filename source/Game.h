#pragma once
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>

namespace ct
{
	class Game
	{
	public:
		Game(sf::RenderTarget& target);

		void Update(float dt);

	private:
		entityx::EntityX m_EntityX;
	};
}
