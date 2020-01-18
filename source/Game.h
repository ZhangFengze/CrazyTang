#pragma once
#include "AnimatedSprite.h"
#include "Player.h"

class Renderer;
class Game
{
public:
	Game();
	void Update(float dt);
	void Render(Renderer&);

private:
	Player m_Player;
	AnimatedSprite m_Bomb = AnimatedSprite::Create({ "bomb/b1.jpg","bomb/b2.jpg","bomb/b3.jpg" });
};
