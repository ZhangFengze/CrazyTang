#pragma once
#include <entityx/entityx.h>
#include "Collider.h"

namespace ct
{
	struct Move
	{
		float acceleration;
		float brakeAcceleration;
		float speed;
		Vector2f size;

		Vector2f velocity;
		bool left = false;
	};

	struct MoveSystem : public entityx::System<MoveSystem>
	{
		void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
	};
}
