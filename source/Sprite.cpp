#include "Sprite.h"
#include "Renderer.h"
#include "Camera.h"

Sprite Sprite::Create(const std::string & file)
{
	Sprite s;
	s.m_Texture = Texture::CreateSafe(file);
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
}

float Sprite::GetScale() const
{
	return m_Scale;
}

void Sprite::Render(Renderer & renderer, Camera & camera)
{
	auto pos = m_Position - camera.GetRect().center;
	renderer.Render(pos.x(), pos.y(), m_Texture);
}
