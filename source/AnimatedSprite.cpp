#include "AnimatedSprite.h"
#include "Renderer.h"

AnimatedSprite AnimatedSprite::Create(const std::initializer_list<std::string>& files)
{
	AnimatedSprite s;
	for (const auto& file : files)
		s.m_Sprites.push_back(Sprite::Create(file));
	return s;
}

AnimatedSprite::AnimatedSprite(AnimatedSprite &&other)
{
	using std::swap;
	swap(m_Pause, other.m_Pause);
	swap(m_Position, other.m_Position);
	swap(m_Scale, other.m_Scale);
	swap(m_AccumulatedTime, other.m_AccumulatedTime);
	swap(m_FrameInterval, other.m_FrameInterval);
	swap(m_Index, other.m_Index);
	swap(m_Sprites, other.m_Sprites);
}

void AnimatedSprite::Render(Renderer& render, Camera& camera)
{
	if (m_Sprites.empty())
		return;
	m_Sprites[m_Index].Render(render, camera);
}

void AnimatedSprite::Update(float dt)
{
	if (m_Sprites.empty())
		return;
	if (m_Pause)
		return;
	m_AccumulatedTime += dt;

	if (m_AccumulatedTime > m_FrameInterval)
	{
		m_Index += m_AccumulatedTime / m_FrameInterval;
		m_Index %= m_Sprites.size();

		m_AccumulatedTime = std::fmod(m_AccumulatedTime, m_FrameInterval);
	}
}

void AnimatedSprite::SetFrameInterval(float f)
{
	assert(f > FLT_EPSILON);
	m_FrameInterval = f;
}

void AnimatedSprite::SetPosition(const Vector2 &pos)
{
	for (auto& sprite : m_Sprites)
		sprite.SetPosition(pos);
	m_Position = pos;
}

Vector2 AnimatedSprite::GetPosition() const
{
	return m_Position;
}

void AnimatedSprite::SetScale(float f)
{
	for (auto& sprite : m_Sprites)
		sprite.SetScale(f);
	m_Scale = f;
}

float AnimatedSprite::GetScale() const
{
	return m_Scale;
}

void AnimatedSprite::SetPause(bool b)
{
	m_Pause = b;
}

AnimatedSprite & AnimatedSprite::operator=(AnimatedSprite && other)
{
	using std::swap;
	swap(m_Pause, other.m_Pause);
	swap(m_Position, other.m_Position);
	swap(m_Scale, other.m_Scale);
	swap(m_AccumulatedTime, other.m_AccumulatedTime);
	swap(m_FrameInterval, other.m_FrameInterval);
	swap(m_Index, other.m_Index);
	swap(m_Sprites, other.m_Sprites);
	return *this;
}
