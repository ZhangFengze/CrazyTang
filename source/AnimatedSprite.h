#pragma once
#include "Sprite.h"
#include <vector>
#include <initializer_list>

namespace ct
{
	struct AnimatedSprite
	{
		std::vector<Sprite> sprites;
		int period;
		int accumulated = 0;
		Vector2f anchor{ 0,0 };

		AnimatedSprite() = default;

		AnimatedSprite(int _period, const std::initializer_list<std::string>& paths)
			:period(std::max(1, _period))
		{
			for (const auto& path : paths)
				sprites.push_back(Sprite{ path });
		}
	};
}
