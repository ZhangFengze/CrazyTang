#pragma once
#include "Math.h"
#include <entityx/entityx.h>

namespace ct
{
	struct Background
	{
		Vector2f originPosition;
		Vector2f moveSpeed;
	};

	struct BackgroundSystem : public entityx::System<BackgroundSystem>
	{
		void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
	};
}
