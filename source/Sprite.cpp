#include "Sprite.h"
#include "Renderer.h"
#include "Camera.h"

Sprite Sprite::Create(const std::string & file)
{
	Sprite s;
	s.m_OriginTexture = Texture::CreateSafe(file);
	return s;
}

void Sprite::SetAnchor(const Vector2 &anchor)
{
	m_Anchor = anchor;
}

void Sprite::SetPosition(const Vector2 &pos)
{
	m_Position = pos;
}

Vector2 Sprite::GetPosition() const
{
	return m_Position;
}

void Sprite::SetScale(float f)
{
	m_Scale = f;
	m_ScaledTexture = m_OriginTexture.GetScaled(m_Scale);
}

float Sprite::GetScale() const
{
	return m_Scale;
}

void Sprite::Render(Renderer & renderer, Camera & camera)
{
	auto pos = m_Position - camera.GetRect().GetTopLeft();
	if (m_ScaledTexture.Valid())
	{
		int x = pos.x() - m_ScaledTexture.GetWidth()*m_Anchor.x();
		int y = pos.y() - m_ScaledTexture.GetHeight()*m_Anchor.y();
		renderer.Render(x, y, m_ScaledTexture);
	}
	else if (m_OriginTexture.Valid())
	{
		int x = pos.x() - m_OriginTexture.GetWidth()*m_Anchor.x();
		int y = pos.y() - m_OriginTexture.GetHeight()*m_Anchor.y();
		renderer.Render(x, y, m_OriginTexture);
	}
}
