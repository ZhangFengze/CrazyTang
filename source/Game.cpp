#include "Game.h"
#include "Renderer.h"

Game::Game()
{
	m_Bomb.SetFrameInterval(1.f / 5.f);
	m_Player.SetFrameInterval(1.f / 10.f);
}

void Game::Update(float dt)
{
	m_Bomb.Update(dt);
	m_Player.Update(dt);
}

void Game::Render(Renderer &r)
{
	m_Bomb.Render(r);
	m_Player.Render(r);
}
