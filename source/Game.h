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
	AnimatedSprite m_Bomb = AnimatedSprite::Create({ "bomb/b1.jpg","bomb/b2.jpg","bomb/b3.jpg" });
	AnimatedSprite m_Player = AnimatedSprite::Create(
		{ "actor/1.jpg","actor/2.jpg","actor/3.jpg","actor/4.jpg" });
};
