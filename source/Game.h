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
};
