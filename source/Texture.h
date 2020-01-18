#pragma once
#include <optional>
#include <string>
#include <SDL.h>

class Texture
{
public:
	static std::optional<Texture> Create(const std::string& file);
	static Texture CreateSafe(const std::string& file);

public:
	Texture() = default;
	~Texture();
	Texture(Texture&&);
	Texture& operator=(Texture&&);

private:
	SDL_Surface* m_Surface = nullptr;
	friend class Renderer;
};

