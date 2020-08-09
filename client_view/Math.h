#pragma once
#include <Eigen>
#include <SFML/System.hpp>

namespace ct
{
	using Eigen::Vector2f;
	using Eigen::Vector2i;
	sf::Vector2f ToSfVector2f(Vector2f);
	Vector2f ToVector2f(sf::Vector2f);
	float Length(Vector2f);
}
