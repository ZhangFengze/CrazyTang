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
}