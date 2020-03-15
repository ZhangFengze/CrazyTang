#include "Texture.h"
#include <unordered_map>

namespace ct
{
	void TextureLoader::update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt)
	{
	}

	const sf::Texture& TextureLoader::GetTexture(const std::string& path)
	{
		auto it = textures_.find(path);
		if (it == textures_.end())
		{
			auto [iter, succ] = textures_.emplace(path, sf::Texture{});
			it = iter;
			it->second.loadFromFile(path);
		}
		return it->second;
	}
}
