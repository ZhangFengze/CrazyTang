#pragma once
#include "Math.h"
#include <SFML/Graphics.hpp>

namespace ct
{
	struct Sprite
	{
		sf::Sprite sprite;
		Vector2f anchor = { 0,0 };

		Sprite() = default;
		Sprite(const std::string& texturePath);
	};
}
