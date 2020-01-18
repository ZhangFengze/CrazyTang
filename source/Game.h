#pragma once
#include "Player.h"
#include "Camera.h"

class Renderer;
class Game
{
public:
	Game();
	void Update(float dt);
	void Render(Renderer&);

private:
	Camera m_Camera;
	Player m_Player;
};
