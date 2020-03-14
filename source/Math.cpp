#include "Math.h"

namespace ct
{
	sf::Vector2f ToSfVector2f(Vector2f vec)
	{
		return { vec.x(),vec.y() };
	}
	Vector2f ToVector2f(sf::Vector2f vec)
	{
		return { vec.x,vec.y };
	}
}
