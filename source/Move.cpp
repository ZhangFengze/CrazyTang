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
		es.each<Move, Transformable>(
			[&](entityx::Entity entity, Move& move, Transformable& transform)
		{
			float delta = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				delta -= 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				delta += 1;

			float velocity = move.velocity.x();
			if (delta != 0)
			{
				velocity += delta * move.acceleration;
				if (std::abs(velocity) > move.speed)
					velocity = velocity / std::abs(velocity) * move.speed;
			}
			else
			{
				if (std::abs(velocity) <= move.brakeAcceleration)
					velocity = 0;
				else
					velocity -= velocity / std::abs(velocity) * move.brakeAcceleration;
			}
			move.velocity = { velocity,0 };

			if (delta != 0)
				move.left = delta < 0;

			if (entity.has_component<Animator>())
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
					entity.component<Animator>()->now = move.left ? "crouch-left" : "crouch-right";
				else if (delta == 0)
					entity.component<Animator>()->now = move.left ? "idle-left" : "idle-right";
				else
					entity.component<Animator>()->now = move.left ? "run-left" : "run-right";
			}

			DoMove(es, move.velocity, move, transform);
			DoMove(es, Vector2f{ 0,3.f }, move, transform);
		});
	}
}
