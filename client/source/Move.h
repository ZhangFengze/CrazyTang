#pragma once
#include <entityx/entityx.h>
#include "Collider.h"

namespace ct
{
	struct Move
	{
		// ground
		float acceleration;
		float brakeAcceleration;
		float speed;

		// jump
		float jumpInitialYSpeed;
		float jumpGravity;

		Vector2f size;

		Vector2f velocity = { 0,0 };
		bool left = false;
	};

	struct MoveSystem : public entityx::System<MoveSystem>
	{
		void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
	};
}
