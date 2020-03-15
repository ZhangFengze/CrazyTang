#pragma once
#include "AnimatedSprite.h"
#include "Math.h"
#include <string>

namespace ct
{
	struct Animator
	{
		std::unordered_map<std::string, AnimatedSprite> states;
		std::string now;
		Vector2f anchor = { 0,0 };
	};
}
