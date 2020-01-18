#pragma once
#include "AnimatedSprite.h"

class Renderer;
class Game
{
public:
	void Update();
	void Render(Renderer&);

private:
	AnimatedSprite m_Player = AnimatedSprite::Create({"player.bmp","player.bmp"});
};
