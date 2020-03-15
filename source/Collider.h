#pragma once
#include "Math.h"

namespace ct
{
	struct Box
	{
		Vector2f topLeft;
		Vector2f size;
	};

	bool Intersect(const Box&, const Box&);
}
