#pragma once
#include <entityx/entityx.h>

namespace ct
{
	struct Move
	{
		float speed;
	};

	struct MoveSystem : public entityx::System<MoveSystem>
	{
		void update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt) override;
	};
}
