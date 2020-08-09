#include "Move.h"
#include "Math.h"
#include "Transformable.h"
#include "Animator.h"
#include <SFML/Window.hpp>

namespace
{
	using namespace ct;
		
	bool Intersect(entityx::EntityManager& entities, const Move& move, const Transformable& trans)
	{
		Box self;
		self.size = move.size;
		self.topLeft = trans.position - Vector2f{ move.size.x() / 2.f, move.size.y() };

		bool intersect = false;
		entities.each<Box>([&](entityx::Entity entity, Box& box)
		{
			if (!intersect && Intersect(self, box))
				intersect = true;
		});
		return intersect;
	}

	bool DoMove(entityx::EntityManager& entities, Vector2f delta, Move& move, Transformable& transform)
	{
		auto target = transform.position + delta;
		if (!Intersect(entities, move, Transformable{ target }))
		{
			transform.position = target;
			return true;
		}
		return false;
	}

	namespace ground
	{
		void UpdateAnimation(entityx::Entity entity, Move& move, float input)
		{
			if (input != 0)
				move.left = input < 0;

			if (entity.has_component<Animator>())
			{
				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
					entity.component<Animator>()->now = move.left ? "crouch-left" : "crouch-right";
				else if (input == 0)
					entity.component<Animator>()->now = move.left ? "idle-left" : "idle-right";
				else
					entity.component<Animator>()->now = move.left ? "run-left" : "run-right";
			}
		}

		void UpdateVelocity(Move& move, float input)
		{
			float velocity = move.velocity.x();
			if (input != 0)
			{
				velocity += input * move.acceleration;
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
		}

		float GetInput()
		{
			float input = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				input -= 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				input += 1;
			return input;
		}

		void HandleJump(Move& move)
		{
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space)
				|| sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
				move.velocity.y() = -move.jumpInitialYSpeed;
		}

		void Update(entityx::EntityManager& entities, entityx::Entity entity, Move& move, Transformable& trans)
		{
			float input = GetInput();

			UpdateVelocity(move, input);
			DoMove(entities, move.velocity, move, trans);
			DoMove(entities, Vector2f{ 0,3.f }, move, trans);

			UpdateAnimation(entity, move, input);

			HandleJump(move);
		}
	}

	namespace jump
	{
		float GetInput()
		{
			float input = 0;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
				input -= 1;
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
				input += 1;
			return input;
		}

		void UpdateVelocity(Move& move, float input)
		{
			float& velocity = move.velocity.x();
			if (input != 0)
			{
				velocity += input * move.acceleration;
				if (std::abs(velocity) > move.speed)
					velocity = velocity / std::abs(velocity) * move.speed;
			}

			move.velocity.y() += move.jumpGravity;
		}

		void UpdateAnimation(entityx::Entity entity, Move& move, float input)
		{
			if (input != 0)
				move.left = input < 0;

			if (entity.has_component<Animator>())
			{
				if (move.velocity.y() < 0)
					entity.component<Animator>()->now = move.left ? "jump-rise-left" : "jump-rise-right";
				else
					entity.component<Animator>()->now = move.left ? "jump-fall-left" : "jump-fall-right";
			}
		}

		void Update(entityx::EntityManager& entities, entityx::Entity entity, Move& move, Transformable& trans)
		{
			float input = GetInput();
			UpdateVelocity(move, input);
			DoMove(entities, { move.velocity.x(),0 }, move, trans);
			if (!DoMove(entities, { 0,move.velocity.y() }, move, trans))
				move.velocity.y() = 0;
			UpdateAnimation(entity, move, input);
		}
	}
}

namespace ct
{
	void MoveSystem::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		entities.each<Move, Transformable>(
			[&](entityx::Entity entity, Move& move, Transformable& trans)
		{
			if (move.velocity.y() == 0)
				ground::Update(entities, entity, move, trans);
			else
				jump::Update(entities, entity, move, trans);
		});
	}
}