#pragma once
#include <string>
#include <optional>
#include <eigen>
#include <SDL.h>

class Renderer;
class Sprite
{
public:
	static std::optional<Sprite> Create(const std::string& file);
	static Sprite CreateSafe(const std::string& file);

public:
	void SetPosition(int x, int y);
	void SetPosition(const Eigen::Vector2i&);
	Eigen::Vector2i GetPosition() const;

	void SetScale(float);
	float GetScale() const;

	int GetWidth() const;
	int GetHeight() const;

public:
	Sprite(Sprite&&);
	~Sprite();
	void Render(Renderer&);
	
private:
	Sprite& operator=(Sprite&&);
	Sprite() = default;
	void ReleaseSurface();

private:
	SDL_Surface* m_Surface = nullptr;
	Eigen::Vector2i m_Position{ 0,0 };
	int m_Width = 0;
	int m_Height = 0;
	float m_Scale = 1.f;

	friend class Renderer;
};
