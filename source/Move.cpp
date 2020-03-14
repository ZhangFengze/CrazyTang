#include "Move.h"
#include "Math.h"
#include "Transformable.h"
#include <SFML/Window.hpp>

namespace ct
{
	void MoveSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		Vector2f delta{ 0,0 };
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
		{
			delta += Vector2f{0,-1};
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
		{
			delta += Vector2f{0,1};
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
		{
			delta += Vector2f{-1,0};
		}
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
		{
			delta += Vector2f{1,0};
		}

		es.each<Move, Transformable>(
			[delta, dt](entityx::Entity entity, Move& move, Transformable& transform)
		{
			transform.position += delta * move.speed * dt;
		});
	}
}