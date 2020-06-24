#pragma once
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <asio.hpp>

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
		asio::io_context io;

	private:
		entityx::EntityX _;
	};
	inline Game* game = nullptr;
}
