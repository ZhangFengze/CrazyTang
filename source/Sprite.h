#pragma once
#include <SFML/Graphics.hpp>

namespace ct
{
	struct Sprite
	{
		sf::Texture texture;
		sf::Sprite sprite;

		Sprite()
		{
			sprite.setTexture(texture);
		}

		Sprite(const std::string& texturePath)
		{
			texture.loadFromFile(texturePath);
			sprite.setTexture(texture);
		}

		Sprite(const Sprite& other)
			:texture(other.texture),
			sprite(other.sprite)
		{
			sprite.setTexture(texture);
		}

		Sprite& operator=(const Sprite& other)
		{
			texture = other.texture;
			sprite = other.sprite;
			sprite.setTexture(texture);
			return *this;
		}
	};
}
