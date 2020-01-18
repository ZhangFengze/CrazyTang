#pragma once
#include <Eigen>

using Vector2 = Eigen::Vector2f;

struct Rect
{
	Vector2 center;
	Vector2 halfExtents;
};
bool Intersect(const Rect&, const Rect&);
