#pragma once
#include "Math.h"
#include <entityx/entityx.h>
#include <SFML/Graphics.hpp>

namespace ct
{
	struct Camera
	{
		Vector2f size;
		entityx::Entity target;
	};

	struct CameraSystem : public entityx::System<CameraSystem>
	{
		CameraSystem(sf::RenderTarget& target) :target_(target) {}

		void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;

		sf::RenderTarget& target_;
	};

}
