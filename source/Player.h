#pragma once
#include "AnimatedSprite.h"
#include "Math.h"
#include "Camera.h"

class Renderer;
class Player
{
public:
	Player();
	void Update(float dt);
	void Render(Renderer&, Camera&);

public:
	void UpdateMove();
	void UpdateBomb();

	Vector2 GetPosition()const;

private:
	AnimatedSprite m_DownAnim = AnimatedSprite::Create(
		{ "actor/1.png","actor/2.png","actor/3.png","actor/4.png" });
	AnimatedSprite m_LeftAnim = AnimatedSprite::Create(
		{ "actor/5.png","actor/6.png","actor/7.png","actor/8.png" });
	AnimatedSprite m_RightAnim = AnimatedSprite::Create(
		{ "actor/9.png","actor/10.png","actor/11.png","actor/12.png" });
	AnimatedSprite m_UpAnim = AnimatedSprite::Create(
		{ "actor/13.png","actor/14.png","actor/15.png","actor/16.png" });
	AnimatedSprite* m_NowAnim = &m_DownAnim;

	Vector2 m_Position{ 0,0 };

	bool m_LastTimeBombKeyDown = false;
	std::vector<AnimatedSprite> m_Bombs;
};
