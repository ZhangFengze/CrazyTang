#include "Texture.h"
#include <SDL_image.h>

namespace
{
	const std::string assetPathPrefix = "../../../asset/";
}

std::optional<Texture> Texture::Create(const std::string & file)
{
	auto surface = IMG_Load((assetPathPrefix + file).c_str());
	if (!surface)
		return std::nullopt;

	Texture sprite;
	sprite.m_Surface = surface;
	return sprite;
}

Texture Texture::CreateSafe(const std::string & file)
{
	return Create(file).value_or(Texture{});
}

int Texture::GetWidth() const
{
	return m_Surface ? m_Surface->w : 0;
}

int Texture::GetHeight() const
{
	return m_Surface ? m_Surface->h : 0;
}

bool Texture::Valid() const
{
	return m_Surface != nullptr;
}

Texture Texture::GetScaled(float scale) const
{
	if (!m_Surface)
		return Texture{};

	auto scaledSurface = SDL_CreateRGBSurfaceWithFormat(0, m_Surface->w*scale, m_Surface->h*scale,
		32, m_Surface->format->format);
	if (!scaledSurface)
		return Texture{};

	SDL_FillRect(scaledSurface, nullptr, SDL_MapRGBA(scaledSurface->format, 0, 0, 0, 0));

	SDL_Rect rect;
	rect.x = rect.y = 0;
	rect.w = scaledSurface->w;
	rect.h = scaledSurface->h;
	SDL_BlitScaled(m_Surface, nullptr, scaledSurface, &rect);

	Texture t;
	t.m_Surface = scaledSurface;
	return t;
}

Texture::~Texture()
{
	if (m_Surface)
	{
		SDL_FreeSurface(m_Surface);
		m_Surface = nullptr;
	}
}

Texture::Texture(Texture &&other)
{
	using std::swap;
	swap(m_Surface, other.m_Surface);
}

Texture & Texture::operator=(Texture &&other)
{
	using std::swap;
	swap(m_Surface, other.m_Surface);
	return *this;
}
