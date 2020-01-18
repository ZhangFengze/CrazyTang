#include "Renderer.h"
#include "Sprite.h"

#include <SDL_image.h>

std::optional<Renderer> Renderer::Create(const std::string & title, int width, int height)
{
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0)
		return std::nullopt;

	int imageFlags = IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF | IMG_INIT_WEBP;
	if (IMG_Init(imageFlags) != imageFlags)
	{
		SDL_Quit();
		return std::nullopt;
	}

	SDL_Window* window =
		SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			width, height, SDL_WINDOW_SHOWN);
	if (!window)
	{
		IMG_Quit();
		SDL_Quit();
		return std::nullopt;
	}

	Renderer rs;
	rs.m_Window = window;
	rs.m_Surface = SDL_GetWindowSurface(rs.m_Window);
	assert(rs.m_Surface);
	return rs;
}

void Renderer::Clear()
{
	SDL_FillRect(m_Surface, nullptr, SDL_MapRGB(m_Surface->format, 255, 255, 255));
}

void Renderer::Render(const Sprite &sp)
{
	auto surface = sp.m_ScaledSurface ? sp.m_ScaledSurface : sp.m_Surface;
	if (surface)
	{
		SDL_Rect dest;
		auto pos = sp.GetPosition();
		dest.x = pos.x();
		dest.y = pos.y();
		SDL_BlitSurface(surface, nullptr, m_Surface, &dest);
	}
}

void Renderer::Commit()
{
	SDL_UpdateWindowSurface(m_Window);
}

Renderer::Renderer(Renderer &&other)
{
	std::swap(m_Window, other.m_Window);
	std::swap(m_Surface, other.m_Surface);
}

Renderer::~Renderer()
{
	Release();
}

void Renderer::Release()
{
	if (m_Window)
	{
		SDL_DestroyWindow(m_Window);
		m_Window = nullptr;
		m_Surface = nullptr;

		IMG_Quit();
		SDL_Quit();
	}
}
