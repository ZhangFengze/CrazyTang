#pragma once
#include <string>
#include <initializer_list>
#include <vector>
#include "Sprite.h"

class Renderer;
class AnimatedSprite
{
public:
	static AnimatedSprite Create(const std::initializer_list<std::string>& files);

public:
	AnimatedSprite(AnimatedSprite&&);

	void Render(Renderer&);

private:
	AnimatedSprite& operator=(AnimatedSprite&&);
	AnimatedSprite() = default;

private:
	size_t m_Index = 0;
	std::vector<Sprite> m_Sprites;
};
