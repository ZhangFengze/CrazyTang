#pragma once
#include <optional>
#include <string>
#include <SDL.h>

class Texture;
class Renderer
{
public:
	static std::optional<Renderer> Create(const std::string& title, int width, int height);

public:
	void Clear();
	void Render(int x, int y, const Texture&);
	void Commit();

public:
	Renderer(Renderer&&);
	Renderer& operator=(Renderer&&) = delete;
	~Renderer();

private:
	Renderer() = default;
	void Release();

private:
	SDL_Window* m_Window = nullptr;
	SDL_Surface* m_Surface = nullptr;
};
