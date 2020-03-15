#include "Move.h"
#include "Math.h"
#include "Transformable.h"
#include <SFML/Window.hpp>

namespace
{
	using namespace ct;
		
	bool DoMove(entityx::EntityManager& entities, Vector2f delta, Move& move, Transformable& transform)
	{
		auto target = transform.position + delta;

		Box self;
		self.size = move.size;
		self.topLeft = target - Vector2f{ move.size.x() / 2.f, move.size.y() };

		bool intersect = false;
		entities.each<Box>([&](entityx::Entity entity, Box& box)
		{
			if (!intersect && Intersect(self, box))
				intersect = true;
		});

		if (!intersect)
		{
			transform.position = target;
			return true;
		}
		return false;
	}
}

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
			[&](entityx::Entity entity, Move& move, Transformable& transform)
		{
			DoMove(es, delta * move.speed * dt, move, transform);
			DoMove(es, Vector2f{ 0,300.f }*dt, move, transform);
		});
	}
}
