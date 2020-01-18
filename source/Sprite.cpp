#include "Sprite.h"

std::optional<Sprite> Sprite::Create(const std::string & file)
{
	auto surface = SDL_LoadBMP(file.c_str());
	if (!surface)
		return std::nullopt;

	Sprite sprite;
	sprite.m_Surface = surface;
	sprite.m_Width = surface->w;
	sprite.m_Height = surface->h;
	return sprite;
}

Sprite Sprite::CreateSafe(const std::string & file)
{
	return Create(file).value_or(Sprite{});
}

void Sprite::SetPosition(int x, int y)
{
	m_Position = { x,y };
}

void Sprite::SetPosition(const Eigen::Vector2i &pos)
{
	m_Position = pos;
}

Eigen::Vector2i Sprite::GetPosition() const
{
	return m_Position;
}

int Sprite::GetWidth() const
{
	return m_Width;
}

int Sprite::GetHeight() const
{
	return m_Height;
}

Sprite::Sprite(Sprite &&other)
{
	m_Surface = other.m_Surface;
	other.m_Surface = nullptr;

	m_Position = other.m_Position;
	m_Width = other.m_Width;
	m_Height = other.m_Height;
}

Sprite & Sprite::operator=(Sprite &&other)
{
	ReleaseSurface();

	m_Surface = other.m_Surface;
	other.m_Surface = nullptr;

	m_Position = other.m_Position;
	m_Width = other.m_Width;
	m_Height = other.m_Height;

	return *this;
}

Sprite::~Sprite()
{
	ReleaseSurface();
}

void Sprite::ReleaseSurface()
{
	if (m_Surface)
	{
		SDL_FreeSurface(m_Surface);
		m_Surface = nullptr;
	}
}
