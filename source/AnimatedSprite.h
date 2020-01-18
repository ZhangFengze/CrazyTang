#pragma once
#include <string>
#include <initializer_list>
#include <vector>
#include "Math.h"
#include "Sprite.h"

class Renderer;
class Camera;
class AnimatedSprite
{
public:
	static AnimatedSprite Create(const std::initializer_list<std::string>& files);

public:
	void SetFrameInterval(float);
	void SetPause(bool);

public:
	// both [0,1]
	void SetAnchor(const Vector2&);

	void SetPosition(const Vector2&);
	Vector2 GetPosition()const;

	void SetScale(float);
	float GetScale() const;

public:
	AnimatedSprite() = default;
	AnimatedSprite(AnimatedSprite&&);
	AnimatedSprite& operator=(AnimatedSprite&&);

	void Render(Renderer&,Camera&);
	void Update(float dt);

private:
	float m_FrameInterval = 1.f / 30.f;
	Vector2 m_Position = Vector2::Zero();
	float m_Scale = 1.f;
	bool m_Pause = false;

	float m_AccumulatedTime = 0.f;
	size_t m_Index = 0;

	std::vector<Sprite> m_Sprites;
};
