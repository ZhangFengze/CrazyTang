#include "Game.h"
#include "Renderer.h"
#include "Input.h"

Game::Game()
{
}

void Game::Update(float dt)
{
	m_Player.Update(dt);
}

void Game::Render(Renderer &r)
{
	m_Player.Render(r);
}
