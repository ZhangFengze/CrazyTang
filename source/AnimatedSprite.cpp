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
	m_Index = (m_Index + 1) % m_Sprites.size();
}

AnimatedSprite & AnimatedSprite::operator=(AnimatedSprite && other)
{
	using std::swap;
	swap(m_Sprites, other.m_Sprites);
	swap(m_Index, other.m_Index);
	return *this;
}
