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
	void SetPosition(const Vector2&);
	Vector2 GetPosition() const;

	void SetScale(float);
	float GetScale() const;

	void Render(Renderer&, Camera&);

public:
	Sprite() = default;
	Sprite(Sprite&&) = default;
	Sprite& operator=(Sprite&&) = default;

private:
	Texture m_Texture;

	Vector2 m_Position{ 0,0 };
	float m_Scale = 1.f;
};
