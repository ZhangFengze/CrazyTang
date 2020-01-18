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
	UpdateMove();
	m_NowAnim->Update(dt);
}

void Player::Render(Renderer &r)
{
	m_NowAnim->Render(r);
}

void Player::UpdateMove()
{
	Eigen::Vector2i move{ 0,0 };
	if (Input::IsKeyDown(Input::Key::Up))
	{
		move = Eigen::Vector2i{0, -1};
		m_NowAnim = &m_UpAnim;
		m_NowAnim->SetPause(false);
	}
	else if (Input::IsKeyDown(Input::Key::Down))
	{
		move = Eigen::Vector2i{0, 1};
		m_NowAnim = &m_DownAnim;
		m_NowAnim->SetPause(false);
	}
	else if (Input::IsKeyDown(Input::Key::Left))
	{
		move = Eigen::Vector2i{-1, 0};
		m_NowAnim = &m_LeftAnim;
		m_NowAnim->SetPause(false);
	}
	else if (Input::IsKeyDown(Input::Key::Right))
	{
		move = Eigen::Vector2i{1, 0};
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
