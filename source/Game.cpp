#include "Game.h"

Game::Game()
{
}

void Game::Update(float dt)
{
	m_Player.Update(dt);
}

void Game::Render(Renderer &render)
{
	m_Player.Render(render, m_Camera);
}
