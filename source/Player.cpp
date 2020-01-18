#include "Player.h"
#include "Input.h"

Player::Player()
{
	float interval = 0.13f;
	m_UpAnim.SetFrameInterval(interval);
	m_DownAnim.SetFrameInterval(interval);
	m_LeftAnim.SetFrameInterval(interval);
	m_RightAnim.SetFrameInterval(interval);

	float scale = 0.2f;
	m_UpAnim.SetScale(scale);
	m_DownAnim.SetScale(scale);
	m_LeftAnim.SetScale(scale);
	m_RightAnim.SetScale(scale);
}

void Player::Update(float dt)
{
	for (auto& bomb : m_Bombs)
		bomb.Update(dt);
	UpdateMove();
	m_NowAnim->Update(dt);
	UpdateBomb();
}

void Player::Render(Renderer &r)
{
	for (auto& bomb : m_Bombs)
		bomb.Render(r);
	m_NowAnim->Render(r);
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
			bomb.SetFrameInterval(1.f / 5.f);
			bomb.SetScale(0.1f);
			bomb.SetPosition(m_Position);
			m_Bombs.push_back(std::move(bomb));
		}
	}
}
