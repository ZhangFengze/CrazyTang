#pragma once
#include "Math.h"

namespace ct
{
	struct Transformable
	{
		Vector2f position = { 0,0 };

		Transformable() = default;
		Transformable(Vector2f position);
	};
}
