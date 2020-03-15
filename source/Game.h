#pragma once
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>

namespace ct
{
	class Game
	{
	public:
		Game();
		void Run();

		entityx::EventManager& events;
		entityx::SystemManager& systems;
		entityx::EntityManager& entities;

	private:
		entityx::EntityX _;
	};
	inline Game* game = nullptr;
}
