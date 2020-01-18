#include "Math.h"

Vector2 Rect::GetTopLeft() const
{
	return center - halfExtents;
}
