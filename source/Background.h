#pragma once
#include "Math.h"
#include <entityx/entityx.h>
#include <string>

namespace ct
{
	struct Background
	{
		float moveSpeed;
		float width;
		float y;
		std::vector<entityx::Entity> sprites;
	};

	entityx::Entity CreateBackground(entityx::EntityManager&,
		const std::string& path, Vector2f scale, float moveSpeed, float y, float cameraWidth);

	struct BackgroundSystem : public entityx::System<BackgroundSystem>
	{
		void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
	};
}
