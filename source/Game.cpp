#include "Game.h"
#include "Renderer.h"

Game::Game()
{
	m_Player.SetFrameInterval(1.f / 5.f);
}

void Game::Update(float dt)
{
	m_Player.Update(dt);
}

void Game::Render(Renderer &r)
{
	m_Player.Render(r);
}
