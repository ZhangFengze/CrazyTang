#pragma once
#include "AnimatedSprite.h"

class Renderer;
class Game
{
public:
	Game();
	void Update(float dt);
	void Render(Renderer&);

private:
	AnimatedSprite m_Player = AnimatedSprite::Create({ "bomb/b1.bmp","bomb/b2.bmp","bomb/b3.bmp" });
};
