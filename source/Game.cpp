#include "Game.h"
#include "Renderer.h"

void Game::Update()
{
}

void Game::Render(Renderer &r)
{
	m_Player.Render(r);
}
