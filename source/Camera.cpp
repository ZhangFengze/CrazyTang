#include "Camera.h"

Camera::Camera()
{
	m_Rect.center = Vector2::Zero();
	m_Rect.halfExtents = { 400,300 };
}

void Camera::SetRect(const Rect &r)
{
	m_Rect = r;
}

Rect Camera::GetRect() const
{
	return m_Rect;
}
