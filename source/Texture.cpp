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
