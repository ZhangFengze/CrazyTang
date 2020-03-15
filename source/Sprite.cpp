#include "Sprite.h"
#include "Game.h"
#include "Texture.h"

namespace ct
{
	Sprite::Sprite(const std::string& texturePath)
	{
		auto& texture = game->systems.system<TextureLoader>()->GetTexture(texturePath);
		sprite.setTexture(texture);
	}

	Sprite::Sprite(const std::filesystem::path& texturePath)
	{
		auto& texture = game->systems.system<TextureLoader>()->GetTexture(texturePath.string());
		sprite.setTexture(texture);
	}
}