#pragma once
#include <string>
#include <initializer_list>
#include <vector>
#include "Math.h"
#include "Sprite.h"

class Renderer;
class AnimatedSprite
{
public:
	static AnimatedSprite Create(const std::initializer_list<std::string>& files);

public:
	void Render(Renderer&);
	void Update(float dt);
	void SetFrameInterval(float);

	void SetPosition(int x, int y);
	void SetPosition(const Vector2&);
	Vector2 GetPosition()const;

	void SetScale(float);
	float GetScale() const;

	void SetPause(bool);

public:
	AnimatedSprite(AnimatedSprite&&);

private:
	AnimatedSprite& operator=(AnimatedSprite&&);
	AnimatedSprite() = default;

private:
	bool m_Pause = false;
	Vector2 m_Position{ 0,0 };
	float m_Scale = 1.f;
	float m_AccumulatedTime = 0.f;
	float m_FrameInterval = 1.f / 30.f;
	size_t m_Index = 0;
	std::vector<Sprite> m_Sprites;
};
