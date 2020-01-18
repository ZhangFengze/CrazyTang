#pragma once
#include "AnimatedSprite.h"
#include <Eigen>

class Renderer;
class Player
{
public:
	Player();
	void Update(float dt);
	void Render(Renderer&);

public:
	void UpdateMove();

private:
	AnimatedSprite m_DownAnim = AnimatedSprite::Create(
		{ "actor/1.jpg","actor/2.jpg","actor/3.jpg","actor/4.jpg" });
	AnimatedSprite m_LeftAnim = AnimatedSprite::Create(
		{ "actor/5.jpg","actor/6.jpg","actor/7.jpg","actor/8.jpg" });
	AnimatedSprite m_RightAnim = AnimatedSprite::Create(
		{ "actor/9.jpg","actor/10.jpg","actor/11.jpg","actor/12.jpg" });
	AnimatedSprite m_UpAnim = AnimatedSprite::Create(
		{ "actor/13.jpg","actor/14.jpg","actor/15.jpg","actor/16.jpg" });
	AnimatedSprite* m_NowAnim = &m_DownAnim;

	Eigen::Vector2i m_Position{ 0,0 };
};
