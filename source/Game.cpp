#include "Game.h"
#include "Renderer.h"

void Game::Update()
{
	m_AccumulatedFrames++;
	if (m_AccumulatedFrames % 10 != 0)
		return;
	m_Player.SetPosition(rand() % 10, rand() % 10);
}

void Game::Render(Renderer &r)
{
	r.Render(m_Player);
}
