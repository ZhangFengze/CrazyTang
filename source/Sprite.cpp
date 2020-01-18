#include "Sprite.h"
#include "Renderer.h"

#include <SDL_image.h>

namespace
{
	const std::string assetPathPrefix = "../../../asset/";
}

std::optional<Sprite> Sprite::Create(const std::string & file)
{
	auto surface = IMG_Load((assetPathPrefix + file).c_str());
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

void Sprite::SetScale(float f)
{
	m_Scale = f;
}

float Sprite::GetScale() const
{
	return m_Scale;
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
	using std::swap;
	swap(m_Surface, other.m_Surface);
	swap(m_ScaledSurface, other.m_ScaledSurface);
	swap(m_Position, other.m_Position);
	swap(m_Width, other.m_Width);
	swap(m_Height, other.m_Height);
	swap(m_Scale, other.m_Scale);
}

Sprite & Sprite::operator=(Sprite &&other)
{
	using std::swap;
	swap(m_Surface, other.m_Surface);
	swap(m_ScaledSurface, other.m_ScaledSurface);
	swap(m_Position, other.m_Position);
	swap(m_Width, other.m_Width);
	swap(m_Height, other.m_Height);
	swap(m_Scale, other.m_Scale);

	return *this;
}

Sprite::~Sprite()
{
	if (m_Surface)
	{
		SDL_FreeSurface(m_Surface);
		m_Surface = nullptr;
	}
	if (m_ScaledSurface)
	{
		SDL_FreeSurface(m_ScaledSurface);
		m_ScaledSurface = nullptr;
	}
}

void Sprite::Render(Renderer &r)
{
	r.Render(*this);
}
