#pragma once
#include <string>
#include "Math.h"
#include "Texture.h"

class Renderer;
class Camera;
class Sprite
{
public:
	static Sprite Create(const std::string& file);

public:
	// both [0,1]
	void SetAnchor(const Vector2&);

	void SetPosition(const Vector2&);
	Vector2 GetPosition() const;

	void SetScale(float);
	float GetScale() const;

public:
	Sprite() = default;
	Sprite(Sprite&&) = default;
	Sprite& operator=(Sprite&&) = default;

	void Render(Renderer&, Camera&);

private:
	Texture m_OriginTexture;
	Texture m_ScaledTexture;

	Vector2 m_Anchor{ 0.5f,0.5f };
	Vector2 m_Position{ 0,0 };
	float m_Scale = 1.f;
};
