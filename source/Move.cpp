#include "Move.h"
#include "Math.h"
#include "Transformable.h"
#include "Animator.h"
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
			if (delta.x() != 0)
				move.left = delta.x() < 0;

			if (entity.has_component<Animator>())
			{
				if (delta.isZero())
					entity.component<Animator>()->now = move.left ? "idle-left" : "idle-right";
				else if (delta.y() > 0)
					entity.component<Animator>()->now = move.left ? "crouch-left" : "crouch-right";
				else
					entity.component<Animator>()->now = move.left ? "run-left" : "run-right";
			}

			DoMove(es, delta * move.speed, move, transform);
			DoMove(es, Vector2f{ 0,3.f }, move, transform);
		});
	}
}
