#include "Game.h"
#include "Renderer.h"
#include "Input.h"

Game::Game()
{
	m_Bomb.SetFrameInterval(1.f / 5.f);
	m_Player.SetFrameInterval(1.f / 10.f);
}

void Game::Update(float dt)
{
	UpdateMove(dt);
	m_Bomb.Update(dt);
	m_Player.Update(dt);
}

void Game::Render(Renderer &r)
{
	m_Bomb.Render(r);
	m_Player.Render(r);
}

void Game::UpdateMove(float dt)
{
	Eigen::Vector2i move{ 0,0 };
	if (Input::IsKeyDown(Input::Key::Up))
		move += Eigen::Vector2i{0, -1};
	if (Input::IsKeyDown(Input::Key::Down))
		move += Eigen::Vector2i{0, 1};
	if (Input::IsKeyDown(Input::Key::Left))
		move += Eigen::Vector2i{-1, 0};
	if (Input::IsKeyDown(Input::Key::Right))
		move += Eigen::Vector2i{1, 0};

	m_Player.SetPosition(m_Player.GetPosition() + move);
}
