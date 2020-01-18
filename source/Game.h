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
	AnimatedSprite m_Bomb = AnimatedSprite::Create({ "bomb/b1.png","bomb/b2.png","bomb/b3.png" });
};
