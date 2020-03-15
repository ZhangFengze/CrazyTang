#pragma once
#include "Math.h"

namespace ct
{
	struct Transformable
	{
		Vector2f position;

		Transformable() = default;
		Transformable(Vector2f position);
	};
}
