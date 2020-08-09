#pragma once
#include <SFML/Graphics.hpp>
#include <entityx/entityx.h>
#include <unordered_map>

namespace ct
{
	class TextureLoader : public entityx::System<TextureLoader>
	{
	public:
		void update(entityx::EntityManager& entities, entityx::EventManager& events, entityx::TimeDelta dt) override;
		const sf::Texture& GetTexture(const std::string&);
	private:
		std::unordered_map<std::string, sf::Texture> textures_;
	};
}
