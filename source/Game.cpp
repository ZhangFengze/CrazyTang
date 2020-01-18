#include "Game.h"
#include "Renderer.h"
#include "Input.h"

Game::Game()
{
	m_Bomb.SetFrameInterval(1.f / 5.f);
	m_Bomb.SetScale(0.2f);
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
