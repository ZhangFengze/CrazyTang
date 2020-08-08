#pragma once
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>

namespace ct
{
	struct RenderSystem :public entityx::System<RenderSystem>
	{
		explicit RenderSystem(sf::RenderTarget& target) : target_(target) {}

		void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;

		sf::RenderTarget& target_;
	};
}
