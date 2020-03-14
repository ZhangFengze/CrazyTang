#pragma once
#include "Sprite.h"
#include <vector>
#include <initializer_list>

namespace ct
{
	struct AnimatedSprite
	{
		std::vector<Sprite> sprites;
		float interval;
		float accumulated;

		AnimatedSprite()
			:interval(0), accumulated(0)
		{
		}

		AnimatedSprite(float _interval, const std::initializer_list<std::string>& paths)
			:interval(_interval), accumulated(0)
		{
			for (const auto& path : paths)
				sprites.push_back(Sprite{ path });
		}
	};
}
