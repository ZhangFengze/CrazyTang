#include "Player.h"
#include "Input.h"

Player::Player()
{
	std::vector<AnimatedSprite*> sprites = { &m_UpAnim,&m_DownAnim,&m_LeftAnim,&m_RightAnim };
	float interval = 0.13f;
	float scale = 0.2f;
	Vector2 anchor{ 0.5f,1.f };
	for (auto sprite : sprites)
	{
		sprite->SetAnchor(anchor);
		sprite->SetFrameInterval(interval);
		sprite->SetScale(scale);
	}
}

void Player::Update(float dt)
{
	for (auto& bomb : m_Bombs)
		bomb.Update(dt);
	UpdateMove();
	m_NowAnim->Update(dt);
	UpdateBomb();
}

void Player::Render(Renderer &render, Camera& camera)
{
	for (auto& bomb : m_Bombs)
		bomb.Render(render, camera);
	m_NowAnim->Render(render, camera);
}

void Player::UpdateMove()
{
	Vector2 move{ 0,0 };
	if (Input::IsKeyDown(Input::Key::Up))
	{
		move = Vector2{0, -1};
		m_NowAnim = &m_UpAnim;
		m_NowAnim->SetPause(false);
	}
	else if (Input::IsKeyDown(Input::Key::Down))
	{
		move = Vector2{0, 1};
		m_NowAnim = &m_DownAnim;
		m_NowAnim->SetPause(false);
	}
	else if (Input::IsKeyDown(Input::Key::Left))
	{
		move = Vector2{-1, 0};
		m_NowAnim = &m_LeftAnim;
		m_NowAnim->SetPause(false);
	}
	else if (Input::IsKeyDown(Input::Key::Right))
	{
		move = Vector2{1, 0};
		m_NowAnim = &m_RightAnim;
		m_NowAnim->SetPause(false);
	}
	else
	{
		m_NowAnim->SetPause(true);
	}

	m_Position += move;
	m_NowAnim->SetPosition(m_Position);
}

void Player::UpdateBomb()
{
	bool down = Input::IsKeyDown(Input::Key::Space);
	if (m_LastTimeBombKeyDown != down)
	{
		m_LastTimeBombKeyDown = down;
		if (down)
		{
			auto bomb = AnimatedSprite::Create({ "bomb/b1.png","bomb/b2.png","bomb/b3.png" });
			bomb.SetAnchor({ 0.5f,1.f });
			bomb.SetFrameInterval(1.f / 5.f);
			bomb.SetScale(0.1f);
			bomb.SetPosition(m_Position);
			m_Bombs.push_back(std::move(bomb));
		}
	}
}
