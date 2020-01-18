#include "Sprite.h"
#include "Renderer.h"
#include "Camera.h"

Sprite Sprite::Create(const std::string & file)
{
	Sprite s;
	s.m_OriginTexture = Texture::CreateSafe(file);
	return s;
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
	auto pos = m_Position - camera.GetRect().center;
	if(m_ScaledTexture.Valid())
		renderer.Render(pos.x(), pos.y(), m_ScaledTexture);
	else if(m_OriginTexture.Valid())
		renderer.Render(pos.x(), pos.y(), m_OriginTexture);
}
