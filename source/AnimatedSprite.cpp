#include "AnimatedSprite.h"
#include "Renderer.h"

AnimatedSprite AnimatedSprite::Create(const std::initializer_list<std::string>& files)
{
	AnimatedSprite s;
	for (const auto& file : files)
		s.m_Sprites.push_back(Sprite::CreateSafe(file));
	return s;
}

AnimatedSprite::AnimatedSprite(AnimatedSprite &&other)
{
	using std::swap;
	swap(m_Sprites, other.m_Sprites);
	swap(m_Index, other.m_Index);
}

void AnimatedSprite::Render(Renderer& r)
{
	if (m_Sprites.empty())
		return;
	r.Render(m_Sprites[m_Index]);
}

void AnimatedSprite::Update(float dt)
{
	if (m_Sprites.empty())
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

void AnimatedSprite::SetPosition(int x, int y)
{
	SetPosition({ x,y });
}

void AnimatedSprite::SetPosition(const Eigen::Vector2i &pos)
{
	for (auto& sprite : m_Sprites)
		sprite.SetPosition(pos);
	m_Position = pos;
}

Eigen::Vector2i AnimatedSprite::GetPosition() const
{
	return m_Position;
}

AnimatedSprite & AnimatedSprite::operator=(AnimatedSprite && other)
{
	using std::swap;
	swap(m_Sprites, other.m_Sprites);
	swap(m_Index, other.m_Index);
	return *this;
}
