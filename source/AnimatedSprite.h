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
		float accumulated = 0;
		Vector2f anchor{ 0,0 };

		AnimatedSprite() = default;

		AnimatedSprite(float _interval, const std::initializer_list<std::string>& paths)
			:interval(_interval)
		{
			for (const auto& path : paths)
				sprites.push_back(Sprite{ path });
		}
	};
}
