#include "Game.h"

Game::Game()
{
	for (int x = -5; x < 5; ++x)
	{
		for (int y = -5; y < 5; ++y)
		{
			auto tile = Sprite::Create("tile/empty.png");
			tile.SetAnchor({ 0,0 });
			tile.SetPosition({ x*100,y*100 });
			tile.SetScale(0.1f);
			m_Tiles.push_back(std::move(tile));
		}
	}
}

void Game::Update(float dt)
{
	m_Player.Update(dt);
	auto rect = m_Camera.GetRect();
	rect.center = m_Player.GetPosition();
	m_Camera.SetRect(rect);
}

void Game::Render(Renderer &render)
{
	for (auto& tile : m_Tiles)
		tile.Render(render, m_Camera);
	m_Player.Render(render, m_Camera);
}
