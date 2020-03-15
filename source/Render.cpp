#include "Render.h"
#include "Sprite.h"
#include "AnimatedSprite.h"
#include "Transformable.h"

namespace
{
	using namespace ct;
	void draw(sf::RenderTarget& target, Sprite& sprite, Transformable& trans, Vector2f anchor)
	{
		auto bounds = sprite.sprite.getGlobalBounds();
		sprite.sprite.setOrigin(bounds.width * anchor.x(), bounds.height * anchor.y());
		sprite.sprite.setPosition(ToSfVector2f(trans.position));
		target.draw(sprite.sprite);
	}
}

namespace ct
{
	void RenderSystem::update(entityx::EntityManager& es, entityx::EventManager& events, entityx::TimeDelta dt)
	{
		es.each<Sprite, Transformable>([this](entityx::Entity entity, Sprite& sprite, Transformable& trans)
		{
			draw(target_, sprite, trans, sprite.anchor);
		});

		es.each<AnimatedSprite, Transformable>([this, dt](entityx::Entity entity, AnimatedSprite& anim, Transformable& trans)
		{
			if (anim.sprites.empty())
				return;
			anim.accumulated += dt;

			size_t index = std::floor(anim.accumulated / anim.interval);
			index = index % anim.sprites.size();

			draw(target_, anim.sprites[index], trans, anim.anchor);
		});
	}
}

