#pragma once
#include "Sprite.h"

class Renderer;
class Game
{
public:
	void Update();
	void Render(Renderer&);

private:
	Sprite m_Player = Sprite::CreateSafe("player.bmp");
	int m_AccumulatedFrames = 0;
};
