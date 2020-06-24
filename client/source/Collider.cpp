#include "Collider.h"

namespace
{
	bool Intersect(float left1, float right1,
		float left2, float right2)
	{
		return !(left1 >= right2 || right1 <= left2);
	}
}

namespace ct
{
	bool Intersect(const Box& b1, const Box& b2)
	{
		if (!::Intersect(b1.topLeft.x(), b1.topLeft.x() + b1.size.x(),
			b2.topLeft.x(), b2.topLeft.x() + b2.size.x()))
			return false;
		if (!::Intersect(b1.topLeft.y(), b1.topLeft.y() + b1.size.y(),
			b2.topLeft.y(), b2.topLeft.y() + b2.size.y()))
			return false;
		return true;
	}
}
